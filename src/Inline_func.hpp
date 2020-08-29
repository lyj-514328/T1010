#include <list>
#include <set>
#include <vector>
#include <iostream>
#include <algorithm>
#include "frontEndInput.hpp"
using namespace std;
class connect_var
{
private:
	int all;
	int i, j;
public:
	connect_var(int all, int init)
	{
		this->all = all;
		this->i = init;
		this->j = all - i;
	}
public:
	int add()
	{
		i++;
		j = all - i;
		return i;
	}
	int get_second()
	{
		return j;
	}
	int get_first()
	{
		return i;
	}

};
class algo_inline_func
{
private:
public:
	void find_function_head_end(vector<pair<TAC*, TAC*>>& func_begin_end, TAC* head)
	{
		if (head == nullptr || func_begin_end.size() != 0)
		{
			cout << "error " << __FILE__ << __LINE__ << endl;
			exit(1);
		}

		for (TAC* to_head = head, *to_end = head; head != NULL; head = head->next)
		{
			if (head->op == TAC_FUNC)
			{
				for (to_end = head, to_head = head; to_end->op != TAC_ENDFUNC; to_end = to_end->next);
				if (to_end == nullptr)
				{
					cout << "error cannot find function end" << __FILE__ << __LINE__ << endl;
				}
				func_begin_end.push_back({ to_head,to_end });
				to_head = to_end;
				head = to_end;
			}
		}
	}
	set<SYMB*>find_func_in_statement(TAC* begin, TAC* end)
	{
		set<SYMB*> v_symbol;
		for (; begin != end; begin = begin->next)
		{
			if (begin->op == TAC_CALL)
			{
				v_symbol.insert(begin->b);
			}
		}
		return v_symbol;
	}
	bool is_a_inline_function(vector<pair<TAC*, TAC*>>& func_begin_end, TAC* head, TAC* func_name, TAC* cofilct)
	{
		assert(func_name->op == TAC_FUNC);
		vector<pair<TAC*, TAC*>>::iterator iter = find_if(func_begin_end.begin(), func_begin_end.end(), [=](pair<TAC*, TAC*>& ptr)->bool
		{
			return ptr.first == func_name;
		});
		if (iter == func_begin_end.end())
		{
			cout << "error cannot find this function" << __FILE__ << __LINE__ << endl;
		}
		set<SYMB*> v_symbol = find_func_in_statement(iter->first, iter->second);


		if (v_symbol.size() == 0)
		{
			return 1;
		}
		if (v_symbol.count(cofilct->a))
		{
			return 0;
		}
		auto iter_func = find_if(v_symbol.begin(), v_symbol.end(), [=, &func_begin_end, &cofilct](SYMB* p_symbol)->bool
		{
			if (is_a_const_func(p_symbol->name))
			{
				return 0;
			}
			auto iter_conf = find_if(func_begin_end.begin(), func_begin_end.end(), [=](pair<TAC*, TAC*>& ptr)->bool
			{
				return ptr.first->a == p_symbol;
			});
			if (iter_conf == func_begin_end.end())
			{
				cout << "error cannot find" << p_symbol->name << __FILE__ << __LINE__ << endl;
				exit(1);
			}
			return !is_a_inline_function(func_begin_end, head, iter_conf->first, cofilct);

		});
		return iter_func == v_symbol.end();
	}
	bool is_a_const_func(string func_name)
	{
		if (func_name.size() == 0)
		{
			return 1;
		}
		if (func_name == "getint" || func_name == "putint" || func_name == "getin" || func_name == "putch" ||
			func_name == "starttime" || func_name == "stoptime" || func_name == "putarray" ||
			func_name == "getint" || func_name == "putf" || func_name == "printf" || func_name == "getarray")
		{
			return 1;
		}
		return 0;
	}
	bool not_used_in_set(set<SYMB*>& v_symbol, SYMB* a)
	{
		auto iter = find_if(v_symbol.begin(), v_symbol.end(), [&](SYMB* ptr)
		{
			return a->type == ptr->type && ptr->name && a->name && a->name == ptr->name;
		});
		return iter == v_symbol.end();
	}
	void insert_item_to_set(set<SYMB*>& v_symbol, SYMB* a, SYMB* b, SYMB* c)
	{
		if (a != NULL && not_used_in_set(v_symbol, a))
		{
			v_symbol.insert(a);
		}
		if (b != NULL && not_used_in_set(v_symbol, b))
		{
			v_symbol.insert(b);
		}
		if (c != NULL && not_used_in_set(v_symbol, c))
		{
			v_symbol.insert(c);
		}
	}
	void get_the_func_symbol(pair<TAC*, TAC*>func_begin_end, set<SYMB*>& v_symbol)
	{
		TAC* begin = func_begin_end.first;
		TAC* end = func_begin_end.second;
		while (begin != end)
		{
			insert_item_to_set(v_symbol, begin->a, begin->b, begin->c);
			begin = begin->next;
		}
	}
	SYMB* copy_a_symbol(SYMB* symbol)
	{
		if (symbol == nullptr)
		{
			return nullptr;
		}
		SYMB* p_symbol = (SYMB*)safe_malloc(sizeof(SYMB), "cpy a symbol");
		p_symbol->type = symbol->type;
		if (symbol->type == SYM_ARRAY || symbol->type == SYM_VAR)
		{
			memcpy(p_symbol, symbol, sizeof(SYMB));
			p_symbol->name = (char*)safe_malloc(50, "copy_a_symbol");
			sprintf(p_symbol->name, "%s", symbol->name);
		}
		else if (symbol->type == SYM_LABEL)
		{
			memcpy(p_symbol, symbol, sizeof(SYMB));
			p_symbol->name = (char*)safe_malloc(50, "copy_a_symbol");
			sprintf(p_symbol->name, "%s", symbol->name);
		}
		else
		{
			memcpy(p_symbol, symbol, sizeof(SYMB));
		}
		return p_symbol;
	}
	TAC* copy_a_tac(TAC* p_statement)
	{
		TAC* ptr = create_a_tac();
		ptr->op = p_statement->op;
		ptr->a = copy_a_symbol(p_statement->a);
		ptr->b = copy_a_symbol(p_statement->b);
		ptr->c = copy_a_symbol(p_statement->c);
		return ptr;
	}
	void rename_symbol(set<SYMB*> set_symbol)
	{
		//label_count	temp_count
		char name[100];
		for_each(set_symbol.begin(), set_symbol.end(), [=, &set_symbol, &name](SYMB* ptr)
		{
			if (ptr->type == SYM_VAR && ptr->name[0] != 'g')
			{
				if (ptr->name[0] == 't')
				{
					sprintf(name, "%c%d", ptr->name[0], temp_count++);
				}
				else
				{
					sprintf(name, "%c%d", ptr->name[0], count_for_rename++);
				}
				sprintf(ptr->name, "%s", name);
			}
			if (ptr->type == SYM_LABEL)
			{
				sprintf(name, "%c%d", ptr->name[0], label_count++);
				sprintf(ptr->name, "%s", name);
			}
		});

	}
	SYMB* get_item_from_set_by_name(vector<SYMB*>& v_symbol, SYMB* ptr)
	{
		if (ptr == nullptr)
		{
			return nullptr;
		}
		if (ptr->type == SYM_ARRAY || ptr->type == SYM_VAR || ptr->type == SYM_LABEL)
		{
			assert(ptr->name != nullptr);
			for (int i = 0; i < v_symbol.size(); i++)
			{
				if (v_symbol[i] && v_symbol[i]->type == ptr->type && strcmp(v_symbol[i]->name, ptr->name) == 0)
				{
					return v_symbol[i];
				}
			}
		}
		return ptr;
	}
	void make_func_var_single(pair<TAC*, TAC*>& pair_func)
	{
		vector<SYMB*>set_symbol;
		for (TAC* p_head = pair_func.first; p_head != pair_func.second->next; p_head = p_head->next)
		{
			set_symbol.push_back(p_head->a);
			set_symbol.push_back(p_head->b);
			set_symbol.push_back(p_head->c);
		}
		for (TAC* p_head = pair_func.first; p_head != pair_func.second->next; p_head = p_head->next)
		{
			p_head->a = get_item_from_set_by_name(set_symbol, p_head->a);
			p_head->b = get_item_from_set_by_name(set_symbol, p_head->b);
			p_head->c = get_item_from_set_by_name(set_symbol, p_head->c);
		}
	}
	pair<TAC*, TAC*> copy_func_tac(pair<TAC*, TAC*>func_begin_end)
	{
		struct copy
		{
			TAC* begin;
			TAC* end;
		}copy;
		copy.begin = create_a_tac();
		copy.end = copy.begin;
		for (; func_begin_end.first != func_begin_end.second; func_begin_end.first = func_begin_end.first->next)
		{
			copy.end->next = copy_a_tac(func_begin_end.first);
			copy.end = copy.end->next;
		}

		copy.end->next = nullptr;

		pair<TAC*, TAC*>temp_begin_end = { copy.begin->next,copy.end };
		make_func_var_single(temp_begin_end);
		free(copy.begin);
		set<SYMB*>set_symbol;
		get_the_func_symbol(temp_begin_end, set_symbol);
		rename_symbol(set_symbol);
		return temp_begin_end;
	}
	void deal_func_ret(pair<TAC*, TAC*>& ref_func, SYMB* ret)
	{
		pair<TAC*, TAC*>func_begin_end = ref_func;
		SYMB* p_symbol = look_up_label(get_label_name(), 1);
		vector<TAC* >v_keep_return;
		for (; func_begin_end.first != func_begin_end.second->next; func_begin_end.first = func_begin_end.first->next)
		{
			if (func_begin_end.first->op == TAC_RETURN)
			{
				v_keep_return.push_back(func_begin_end.first);
			}
		}
		for_each(v_keep_return.begin(), v_keep_return.end(), [=, &ret, &p_symbol](TAC* ptr)
		{
			if (ptr->a)
			{
				assert(ret != nullptr);
				ptr->op = TAC_COPY;
				sprintf(ret->name, "u%d", count_for_rename++);
				ret->name[0] = 'u';
				ptr->b = ptr->a;
				ptr->a = ret;
				insert_a_node(ptr, create_a_node(p_symbol, NULL, NULL, TAC_GOTO));
			}
			else
			{
				ptr->op = TAC_GOTO;
				ptr->a = p_symbol;
			}
		});
		func_begin_end.second = create_a_node(p_symbol, nullptr, nullptr, TAC_LABEL);

		ref_func.second = func_begin_end.second;
		join_tac(ref_func.first, ref_func.second);
	}

	void deal_func_formal(pair<TAC*, TAC*>& ref_func, TAC* call_func)
	{
		vector<TAC*> func_actual, func_formal;
		TAC* begin = ref_func.first;
		TAC* end = ref_func.second;
		for (TAC* p_actual = call_func->prev; p_actual->op == TAC_ACTUAL; p_actual = p_actual->prev)
		{
			func_actual.push_back(p_actual);
		}
		for (TAC* formal = begin->next->next; formal->op == TAC_FORMAL; formal = formal->next)
		{
			func_formal.push_back(formal);
		}
		assert(func_formal.size() == func_actual.size());
		for (connect_var var(func_formal.size() - 1, 0); var.get_first() < func_formal.size(); var.add())
		{
			if (func_formal[var.get_first()]->a->type == SYM_VAR || func_formal[var.get_first()]->a->type == SYM_INT)
			{
				func_formal[var.get_first()]->op = TAC_COPY;
				func_formal[var.get_first()]->b = func_actual[var.get_second()]->a;
			}
			else
			{
				SYMB* ptr_formal = func_formal[var.get_first()]->a;
				SYMB* ptr_actual = func_actual[var.get_second()]->a;
				TAC* ptr_lea_statement = find_statement_lea(func_actual[var.get_second()], ptr_actual);
				SYMB* ptr_lea_arr = ptr_lea_statement->b;
				SYMB* ptr_lea_dis = ptr_lea_statement->c;
				set<TAC*> set_arr_use = find_statement_arr_use(func_formal[var.get_first()], ptr_formal);
				make_array_rename(set_arr_use, ptr_lea_arr, ptr_formal, ptr_lea_dis);
			}
		}

	}
	void make_array_rename(set<TAC*>& set_arr_use, SYMB* source, SYMB* destination, SYMB* dis)
	{
		SYMB* dis_source = nullptr;
		for_each(set_arr_use.begin(), set_arr_use.end(), [&](TAC* ptr)
		{
			dis_source = look_up_var(get_temp_var_name(), 1);
			dis_source->name[0] = 't';
			if (ptr->op == TAC_ARR_L)
			{
				assert(destination == ptr->a);
				insert_a_node(ptr->prev, create_a_node(dis_source, dis, ptr->b, TAC_ADD));
				ptr->a = source;
				ptr->b = dis_source;
			}
			if (ptr->op == TAC_ARR_R || ptr->op == TAC_LEA)
			{
				assert(destination == ptr->b);
				insert_a_node(ptr->prev, create_a_node(dis_source, dis, ptr->c, TAC_ADD));
				ptr->b = source;
				ptr->c = dis_source;
			}
		});
	}
	set<TAC*>find_statement_arr_use(TAC* statement, SYMB* symbol)
	{
		set<TAC*>set_arr_use;
		for (; statement != nullptr && statement->op != TAC_ENDFUNC; statement = statement->next)
		{
			if (statement->op == TAC_ARR_L && statement->a == symbol)
			{
				set_arr_use.insert(statement);	continue;
			}
			if (statement->op == TAC_ARR_R && statement->b == symbol)
			{
				set_arr_use.insert(statement);	continue;
			}
			if (statement->op == TAC_LEA && statement->b == symbol)
			{
				set_arr_use.insert(statement);	continue;
			}
		}
		return set_arr_use;
	}
	TAC* find_statement_lea(TAC* p_statement, SYMB* symbol)
	{
		for (; p_statement != nullptr && p_statement->a != symbol || p_statement->op == TAC_ACTUAL; p_statement = p_statement->prev);
		assert(p_statement != nullptr);
		assert(p_statement->op == TAC_COPY);
		symbol = p_statement->b;
		for (; p_statement != nullptr && p_statement->a != symbol; p_statement = p_statement->prev);
		assert(p_statement->op == TAC_LEA);
		assert(p_statement->c != nullptr);
		return p_statement;
	}
	TAC* inline_the_func(TAC* p_statement, pair<TAC*, TAC*>func_begin_end, set<SYMB*>& v_symbol)
	{
		TAC* begin = func_begin_end.first;
		TAC* end = func_begin_end.second;
	}
	void insert_a_node(TAC* pre, TAC* node)
	{
		if (pre->next)
		{
			pre->next->prev = node;
		}
		node->next = pre->next;

		node->prev = pre;
		pre->next = node;
	}
};

class InlineFunction
{
private:
	TAC* _inline_tac_head;
	algo_inline_func obj_function;
	vector<pair<TAC*, TAC*>> func_begin_end;
public:
	InlineFunction(TAC* tac_head) :_inline_tac_head(tac_head)
	{
		get_head_func();
	};
	void get_head_func(void)
	{
		obj_function.find_function_head_end(func_begin_end, _inline_tac_head);
	}
	void replace_statement_use_func(pair<TAC*, TAC*>& ref_func, TAC* call_func)
	{

		ref_func.first->prev = call_func->prev;
		call_func->next->prev = ref_func.second;
		call_func->prev->next = ref_func.first;
		ref_func.second->next = call_func->next;
	}
	void delete_a_tac_node(TAC* p_statement)
	{
		TAC* node_prev = p_statement->prev;
		TAC* node_next = p_statement->next;
		node_prev->next = node_next;
		node_next->prev = node_prev;
		free(p_statement);
	}
	void delete_formal_actual(TAC* func_name)
	{
		delete_a_tac_node(func_name->next);
		while (func_name->prev->op == TAC_ACTUAL)
		{
			delete_a_tac_node(func_name->prev);
		}
		assert(func_name->prev->op != TAC_ACTUAL);
		for (; func_name->next->op != TAC_ENDFUNC; func_name = func_name->next)
		{
			if (func_name->op == TAC_FORMAL && func_name->a->type == SYM_ARRAY)
			{
				TAC* keep = func_name;
				func_name->next->prev = func_name->prev;
				func_name->prev->next = func_name->next;
				func_name = func_name->prev;
				free(keep);
			}
		}
	}
	void make_tac_prev(TAC* begin, TAC* end)
	{
		TAC* prev = nullptr;
		for (prev = begin, begin = begin->next; begin != end->next; begin = begin->next, prev = prev->next)
		{
			begin->prev = prev;
		}
	}
	void get_out_func(pair<TAC*, TAC*>& pair_func)
	{
		assert(pair_func.first != nullptr);
		assert(pair_func.second != nullptr);
		assert(pair_func.first->op == TAC_FUNC);
		if (pair_func.first->prev == nullptr)
		{
			tac_head = pair_func.second->next;
			return;
		}
		if (strcmp(pair_func.first->a->name, "main") == 0)
		{
			return;
		}
		TAC* p_begin = pair_func.first->prev;
		TAC* p_end = pair_func.second->next;
		p_begin->next = pair_func.second->next;
		pair_func.second->next->prev = p_begin;
	}
	void declare_var(pair<TAC*, TAC*> pair_func)
	{
		TAC* to_begin = pair_func.first;
		TAC* insert = to_begin->next;
		for (; insert->next->op == TAC_FORMAL; insert = insert->next);
		TAC* to_end = pair_func.second;
		set<SYMB*>had_decl;
		for (TAC* head = to_begin; head != to_end; head = head->next)
		{
			if (head->op == TAC_VAR || head->op == TAC_ARR || head->op == TAC_FORMAL)
			{
				assert(head->a != nullptr);
				had_decl.insert(head->a);
			}
		}
		set<SYMB*>item_used;
		for (TAC* head = to_begin; head != to_end; head = head->next)
		{
			insert_item_to_set(item_used, head->a);
			insert_item_to_set(item_used, head->b);
			insert_item_to_set(item_used, head->c);
		}
		for_each(item_used.begin(), item_used.end(), [&](SYMB* symbol)
		{
			if (had_decl.count(symbol) == 0 && symbol->name[0] != 'g')
			{
				obj_function.insert_a_node(insert, create_a_node(symbol, nullptr, nullptr, TAC_VAR));
			}
		});
	}

	void insert_item_to_set(set<SYMB*>& set_ref, SYMB* ptr)
	{
		if (ptr == nullptr)
		{
			return;
		}
		if (ptr->type == SYM_VAR)
		{
			set_ref.insert(ptr);
		}
	}
	void tidy_declare(void)
	{
		this->func_begin_end.clear();
		obj_function.find_function_head_end(this->func_begin_end, tac_head);
		for_each(func_begin_end.begin(), func_begin_end.end(), [&](pair<TAC*, TAC*>ptr)
		{
			TAC* begin = ptr.first;
			TAC* end = ptr.second;
			list<TAC*>keep_decl;
			for (; begin != end; begin = begin->next)
			{
				if (begin->op == TAC_VAR || begin->op == TAC_ARR)
				{
					keep_decl.push_back(begin);
				}
			}
			auto iter = keep_decl.begin();
			for (; iter != keep_decl.end(); )
			{
				if ((*iter) == nullptr)
				{
					iter++;
					continue;
				}
				SYMB* symbol = (*iter)->a;
				iter++;
				for (auto cur = iter; cur != keep_decl.end(); cur++)
				{
					if ((*cur) && (*cur)->a == symbol)
					{
						(*cur)->next->prev = (*cur)->prev;
						(*cur)->prev->next = (*cur)->next;
						(*cur) = nullptr;
					}
				}
			}
		});
	}
	pair<TAC*, TAC*> get_statement_func(TAC* p_statement)
	{
		pair<TAC*, TAC*> func;
		TAC* ptr = p_statement;
		while (p_statement && p_statement->op != TAC_FUNC)
		{
			p_statement = p_statement->prev;
		}
		assert(p_statement->op == TAC_FUNC);
		func.first = p_statement;
		while (ptr && ptr->op != TAC_ENDFUNC)
		{
			ptr = ptr->next;
		}
		assert(ptr->op == TAC_ENDFUNC);
		func.second = ptr;
		return func;

	}
	bool isnot_so_long(pair<TAC*, TAC*> pair_func)
	{
		return 1;
		int count;
		for (count = 0; pair_func.first != pair_func.second; pair_func.first = pair_func.first->next)
		{
			count++;
		}
		return count <= 50;
	}
	void inline_all_function(void)
	{
		for_each(func_begin_end.begin(), func_begin_end.end(), [&](pair<TAC*, TAC*>& pair_func)
		{
			if (isnot_so_long(pair_func) && strcmp(pair_func.first->a->name, "main") && obj_function.is_a_inline_function(func_begin_end, _inline_tac_head, pair_func.first, pair_func.first))
			{
				//	pair<TAC*, TAC*>func_copy = obj_function.copy_func_tac(pair_func);
				vector<TAC*> set_satatement;
				for (TAC* begin_find = _inline_tac_head; begin_find != nullptr; begin_find = begin_find->next)
				{
					if (begin_find->op == TAC_CALL && begin_find->b == pair_func.first->a)
					{
						set_satatement.push_back(begin_find);
					}
				}
				for (int i = 0; i < set_satatement.size(); i++)
				{
					pair<TAC*, TAC*>keep_map = get_statement_func(set_satatement[i]);
					pair<TAC*, TAC*> pair_copy = obj_function.copy_func_tac(pair_func);
					make_tac_prev(pair_copy.first, pair_copy.second);
					obj_function.deal_func_ret(pair_copy, set_satatement[i]->a);
					make_tac_prev(pair_copy.first, pair_copy.second);
					obj_function.deal_func_formal(pair_copy, set_satatement[i]);
					make_tac_prev(pair_copy.first, pair_copy.second);
					replace_statement_use_func(pair_copy, set_satatement[i]);
					delete_formal_actual(pair_copy.first);

					make_tac_prev(pair_copy.first, pair_copy.second);
					declare_var(keep_map);
					delete_a_tac_node(pair_copy.first);
				}
				get_out_func(pair_func);
			}
		});
		tidy_declare();
	}
};