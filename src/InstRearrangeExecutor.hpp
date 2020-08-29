#include "globalPolicyExecutor.hpp"
class InstRearrangeExecutor:
public GlobalPolicyExecutor
{
private:
    void InstRearrangeForABlock(BasicBlock* basicBlock);
public:
    InstRearrangeExecutor();
    ~InstRearrangeExecutor();
    void printInfoOfOptimizer(void);
    bool runOptimizer(FunctionBlock* block);//运行优化器
};

InstRearrangeExecutor::InstRearrangeExecutor(/* args */)
{
    m_name = "指令重排器";
}

InstRearrangeExecutor::~InstRearrangeExecutor()
{
}

void InstRearrangeExecutor::printInfoOfOptimizer(void)
{

}

bool InstRearrangeExecutor::runOptimizer(FunctionBlock* block)
{
    m_block = block;
    vector<BasicBlock*>& basicBlocks = m_block->getBasicBlocks();
    for(uint i = 0;i < basicBlocks.size();i++)
    {

    }
    return false;//指令重排一次就好
}

//关于指令重排,insert指令需要关心，但无需重拍
//insert的左值有用，但只用于
void InstRearrangeExecutor::InstRearrangeForABlock(BasicBlock* basicBlock)
{
    SsaTac* tacHead = basicBlock->getTacHead();
    SsaTac* tacTail = basicBlock->getTacTail();
    if(basicBlock->getInstNum() < 3)return;//0 1 2 指令数不需要重排
    if(tacHead == NULL)return;//保险
    vector<SsaSymb*> insertSymbList;//最开始的insert
    insertSymbList.clear();
    SsaTac* tacHeadHead = new SsaTac();
    tacHeadHead->next = tacHead;
    SsaTac* curTac = tacHeadHead;
    do
    {
        curTac = curTac->next;
        if(curTac->type == TAC_INSERT)//如果是insert，那么加入左值
            insertSymbList.push_back(curTac->first);
        else if(curTac->type == TAC_LABEL)continue;

    }while(curTac != tacTail);

    vector<vector<uint>> graph;
    vector<uint> IndegreeOfPoint;
    IndegreeOfPoint.clear();
    graph.clear();
}