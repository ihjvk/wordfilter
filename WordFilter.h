#include <map>
#include <string>
#include <vector>
using namespace std;
#define std_map std::map
#define SPECIAL_KEY	("*")
class WordFilterNode
{
private:
	enum Flag
	{
		FLAG_END = 1,		// 关键词是否结束(比如 关键词有你,你好 那么匹配到你时则命中)
		FLAG_MATCHALL = 2,	// 此标记用来检查A*B类词 比如毒*品为关键词,毒XXX品应该被检测 
	};
public:
	friend class Tree;
	typedef std_map<std::string, WordFilterNode*> _TreeMap;
	typedef std_map<std::string,WordFilterNode*>::iterator _TreeMapIterator;
	// 关键字
	std::string	m_character;	
	// 子树
	_TreeMap	m_map;
	
	int			m_flag;
public:
	void Clear()
	{
		m_map.clear();
		m_character.clear();
		m_flag = 0;
	}
	WordFilterNode(std::string character)
	{
		Clear();
		m_character.assign(character);
	}
	WordFilterNode()
	{
		Clear();
	};

	WordFilterNode* FindChild(std::string& nextCharacter)
	{
		_TreeMapIterator TreeMapIt = m_map.find(nextCharacter);   
		if (TreeMapIt == m_map.end())
		{
			return NULL;
		}
		return TreeMapIt->second;
	}

	WordFilterNode* InsertChild(string& nextCharacter)
	{
		if (NULL != FindChild(nextCharacter))
		{// already have,return;
			return NULL;
		}
		// add node and return it;
		WordFilterNode* pNode = new WordFilterNode(nextCharacter);
		if (NULL == pNode)
		{
			return NULL;
		}
		m_map.insert(make_pair(nextCharacter, pNode));
		return pNode;		
	}
	bool	IsEnd()
	{
		return (m_flag&FLAG_END) == FLAG_END;
	}
	
	void	SetEnd()
	{
		m_flag |= FLAG_END;
	}

	void	SetMatchAll()
	{
		m_flag |= FLAG_MATCHALL;
	}

	bool	IsMatchAll()
	{
		return (m_flag&FLAG_MATCHALL) == FLAG_MATCHALL;
	}
};

class WordFilterTree
{
public:
	int m_nBeginIdx;	// 找到的敏感词开始idx;
	int m_nCount;		// 当前查找的一个敏感词字节长度
	WordFilterTree()
	{
		Clear();
		m_Root.Clear();
	};
	~WordFilterTree()
	{
		Clear();
		m_Root.Clear();
	}
	void Clear()
	{
		m_nCount = 0;
		m_nBeginIdx = 0;
	}
	bool Insert(vector<std::string>& keywords)
	{
		return insert(&m_Root, keywords);
	}

	WordFilterNode* Find(vector<std::string>& keywords/*string& keyword*/)
	{
		Clear();
		return find(&m_Root,keywords);
	}

private:
	WordFilterNode m_Root;
	bool insert(WordFilterNode* parent, vector<std::string>& keywordVec)
	{
		// 遍历关键字按汉字,字符拆分的vector
		for (size_t i = 0; i < keywordVec.size(); i++)
		{
			if (NULL == parent)
			{// 出错了
				return false;
			}
			std::string& wordstr = keywordVec[i];	
			if (wordstr == SPECIAL_KEY)
			{
				continue;
			}
			std::string nextstr; 
			if (i+1 < keywordVec.size())
			{
				nextstr = keywordVec[i+1];	
			}
			WordFilterNode* pNode = parent->FindChild(wordstr);

			if (NULL == pNode)
			{
				parent = parent->InsertChild(wordstr);
			}
			else
			{
				parent = pNode;
			}
			if (nextstr == SPECIAL_KEY)
			{
				parent->SetMatchAll();
			}
			if (i == keywordVec.size() - 1)
			{// 最后一个
				parent->SetEnd();
			}
		}
		return true;
	}
	WordFilterNode* find(WordFilterNode* parent,vector<std::string>& words)
	{	
		int nLength = 0; // 已检查过的字节长度
		for(size_t i = 0; i < words.size(); /*i++*/)
		{
			if (NULL == parent)
			{// 出错
				return NULL;
			}
			std::string& wordstr = words[i];

			WordFilterNode* findNode = parent->FindChild(wordstr);
			if (NULL == findNode)
			{// 
				if(!parent->IsMatchAll())
				{
					m_nBeginIdx = 0;
					m_nCount = 0;
				}
				if (parent == &m_Root || parent->IsMatchAll())
				{// 跳过这个字,从后面的词重头找
					i++;
					nLength += wordstr.size();
					if (parent->IsMatchAll())
					{
						m_nCount += wordstr.size();
					}
					continue;
				}
				else
				{// 表明词组不匹配,从这个字开始重头找
					parent = &m_Root;
				}
				continue;
			}
			else
			{
				if (m_nCount == 0)
				{// 记录敏感词开始idx
					m_nBeginIdx = nLength ;
				}
				m_nCount+= wordstr.size();
				if(findNode->IsEnd())
				{// 对应词组结束，则判断该词为敏感词
					return findNode;
				}
				// 在子树里继续找
				parent = findNode;
				nLength += wordstr.size();
				i++;
			}
		}
		return NULL;
	}

};

class WordFilter
{
private:
	static WordFilterTree m_tree;
public:
	static bool Load(const char* fileName);
	static bool CheckWord(std::string& source);

	static void test();

	// 将关键词按字节数切分(中文,英文,符号)
	static bool	SplitLine(std::string& word, vector<std::string>& splitWord);
};