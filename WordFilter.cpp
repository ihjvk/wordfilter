#include "WordFilter.h"
#include <iostream>
#include <fstream>
#include <algorithm>
#define MAX_LINE_BYTES	(1024*64)
#define WORD_BYTES		(100)

#define ONE_BYTES		(1)
#define	TWO_BYTES		(2)
#define THTEE_BYTES		(3)
WordFilterTree WordFilter::m_tree;
bool WordFilter::Load( const char* filename )
{
	// 用binary方式读取,否则windows下\r\n会被转为\n,linux会在关键词里留下\r
	ifstream keywordsFile(filename,ios::in|ios::binary);
	if (!keywordsFile.is_open())
	{
		cout << "file[" << filename << "] open fail";
		return false;
	}
	int i = 0;
	while(!keywordsFile.eof())
	{
		static char szWord[WORD_BYTES];
		keywordsFile.getline(szWord,WORD_BYTES,'\n');
		
		std::string str(szWord);
		// 按中文,英文切分敏感词
		vector<std::string> words;
		if (!WordFilter::SplitLine(str, words))
		{
			cout << "splitline " << str << " error!" << endl;
			continue;
		}
		m_tree.Insert(words);
	}
	keywordsFile.close();
	return true;
}

bool WordFilter::CheckWord( std::string& source )
{
	vector<std::string> words;
	if (!WordFilter::SplitLine(source, words))
	{
		return false;
	}
	if (m_tree.Find(words) != NULL)    
	{// 发现敏感词
		if (m_tree.m_nBeginIdx < 0 || m_tree.m_nBeginIdx >= (int)source.size() || (m_tree.m_nBeginIdx + m_tree.m_nCount) > (int)source.size())
		{
			return false;
		}
 		source.replace(m_tree.m_nBeginIdx,m_tree.m_nCount,"**");
		return false;
	}
	return true;
}

void WordFilter::test()
{
	ifstream inputFile("input.txt",ios::in|ios::binary);
	if (!inputFile.is_open())
	{
		return;
	}
	ofstream outputFile("output.txt",ios::out);
	int nCount = 0;
	{
		while(!inputFile.eof())
		{
			char ch[WORD_BYTES];
			inputFile.getline(ch,WORD_BYTES,'\n');
			cout << ch << " has key word" << endl;
			std::string str = std::string(ch);
			CheckWord(str);
			nCount++;
			outputFile<<str<<endl;

		}
	}
	inputFile.close();
	outputFile.close();
}

bool WordFilter::SplitLine( std::string& word, vector<std::string>& splitWord )
{
	for(size_t cur = 0;cur < word.size(); /*cur++*/)
	{        
		char t = word[cur];
		std::string lines;
		if((t&0xE0) == 0xE0)
		{// 3byte
			if (cur + THTEE_BYTES > word.size())
			{// 错误！
				return false;
			}
			lines = word.substr(cur, THTEE_BYTES);
			cur += THTEE_BYTES;	
		}
		else if((t&0xC0) == 0xC0)
		{// 2byte
			if (cur + TWO_BYTES > word.size())
			{// 错误！
				return false;
			}
			lines = word.substr(cur, TWO_BYTES);
			cur += TWO_BYTES;
		}
		else
		{// 1byte
			if (cur + ONE_BYTES > word.size())
			{// 错误！
				return false;
			}
			lines = word.substr(cur, ONE_BYTES);
			if (lines == "\r")
			{// 忽略换行符
				cur += ONE_BYTES;
				continue;
			}
			transform(lines.begin(), lines.end(), lines.begin(), (int (*)(int))toupper);
			cur += ONE_BYTES;
		}
		splitWord.push_back(lines);
	}
	return true;
}

