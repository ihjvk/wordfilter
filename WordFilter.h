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
		FLAG_END = 1,		// �ؼ����Ƿ����(���� �ؼ�������,��� ��ôƥ�䵽��ʱ������)
		FLAG_MATCHALL = 2,	// �˱���������A*B��� ���綾*ƷΪ�ؼ���,��XXXƷӦ�ñ���� 
	};
public:
	friend class Tree;
	typedef std_map<std::string, WordFilterNode*> _TreeMap;
	typedef std_map<std::string,WordFilterNode*>::iterator _TreeMapIterator;
	// �ؼ���
	std::string	m_character;	
	// ����
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
	int m_nBeginIdx;	// �ҵ������дʿ�ʼidx;
	int m_nCount;		// ��ǰ���ҵ�һ�����д��ֽڳ���
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
		// �����ؼ��ְ�����,�ַ���ֵ�vector
		for (size_t i = 0; i < keywordVec.size(); i++)
		{
			if (NULL == parent)
			{// ������
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
			{// ���һ��
				parent->SetEnd();
			}
		}
		return true;
	}
	WordFilterNode* find(WordFilterNode* parent,vector<std::string>& words)
	{	
		int nLength = 0; // �Ѽ������ֽڳ���
		for(size_t i = 0; i < words.size(); /*i++*/)
		{
			if (NULL == parent)
			{// ����
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
				{// ���������,�Ӻ���Ĵ���ͷ��
					i++;
					nLength += wordstr.size();
					if (parent->IsMatchAll())
					{
						m_nCount += wordstr.size();
					}
					continue;
				}
				else
				{// �������鲻ƥ��,������ֿ�ʼ��ͷ��
					parent = &m_Root;
				}
				continue;
			}
			else
			{
				if (m_nCount == 0)
				{// ��¼���дʿ�ʼidx
					m_nBeginIdx = nLength ;
				}
				m_nCount+= wordstr.size();
				if(findNode->IsEnd())
				{// ��Ӧ������������жϸô�Ϊ���д�
					return findNode;
				}
				// �������������
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

	// ���ؼ��ʰ��ֽ����з�(����,Ӣ��,����)
	static bool	SplitLine(std::string& word, vector<std::string>& splitWord);
};