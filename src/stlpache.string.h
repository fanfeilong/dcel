#ifndef DCEL_STLPACHE_STRING_H
#define DCEL_STLPACHE_STRING_H

#include <string>
#include <iterator>
#include <sstream>

using namespace std;

namespace stlpache
{
	inline string&  left_trim(string   &ss)   
	{   
		string::iterator   p=find_if(ss.begin(),ss.end(),not1(ptr_fun(isspace)));   
		ss.erase(ss.begin(),p);   
		return  ss;   
	}   
	
	inline string&  right_trim(string   &ss)   
	{   
		string::reverse_iterator  p=find_if(ss.rbegin(),ss.rend(),not1(ptr_fun(isspace)));   
		ss.erase(p.base(),ss.end());   
		return   ss;   
	}   
	
	inline string&  trim(string   &st)   
	{   
		left_trim(right_trim(st));   
		return   st;   
	}   

	inline std::string itos(int i)
	{
		std::stringstream ss;
		std::string s;
		ss << i;
		s = ss.str();

		return s;
	}

	inline std::string ws2s(const std::wstring& ws)
	{
		std::string curLocale = setlocale(LC_ALL, NULL);        // curLocale = "C";
		setlocale(LC_ALL, "chs");
		const wchar_t* _Source = ws.c_str();
		size_t _Dsize = 2 * ws.size() + 1;
		char *_Dest = new char[_Dsize];
		memset(_Dest,0,_Dsize);
		wcstombs(_Dest,_Source,_Dsize);
		std::string result = _Dest;
		delete []_Dest;
		setlocale(LC_ALL, curLocale.c_str());
		return result;
	}

	inline std::wstring s2ws(const std::string& s)
	{
		setlocale(LC_ALL, "chs"); 
		const char* _Source = s.c_str();
		size_t _Dsize = s.size() + 1;
		wchar_t *_Dest = new wchar_t[_Dsize];
		wmemset(_Dest, 0, _Dsize);
		mbstowcs(_Dest,_Source,_Dsize);
		std::wstring result = _Dest;
		delete []_Dest;
		setlocale(LC_ALL, "C");
		return result;
	}

	struct string_token_iterator : public std::iterator<std::input_iterator_tag, std::string>
	{
	public:
		string_token_iterator() : str(0), start(0), end(0) {}
		string_token_iterator(const std::string & str_, const char * separator_ = " ") 
			:separator(separator_),str(&str_),end(0)
		{
			find_next();
		}
		string_token_iterator(const string_token_iterator & rhs) 
			:separator(rhs.separator),str(rhs.str),start(rhs.start),end(rhs.end)
		{

		}

		string_token_iterator & operator++()
		{
			find_next();
			return *this;
		}
		string_token_iterator operator++(int)
		{
			string_token_iterator temp(*this);
			++(*this);
			return temp;
		}
		std::string operator*() const
		{
			return std::string(*str, start, end - start);
		}
		bool operator==(const string_token_iterator & rhs) const
		{
			return (rhs.str == str && rhs.start == start && rhs.end == end);
		}
		bool operator!=(const string_token_iterator & rhs) const
		{
			return !(rhs == *this);
		}
	private:
		void find_next(void)
		{
			start = str->find_first_not_of(separator, end);
			if(start == std::string::npos)
			{
				start = end = 0;
				str = 0;
				return;
			}

			end = str->find_first_of(separator, start);
		}
		const char * separator;
		const std::string * str;
		std::string::size_type start;
		std::string::size_type end;
	};

	struct line
	{
		std::string data;
		operator std::string() const
		{
			return data;
		}
		friend std::istream& operator>>(std::istream& in,line& linestr)
		{
			std::getline(in,linestr.data);
			return in;
		}
		friend std::ostream& operator<<(std::ostream& out,line const& linestr)
		{
			out<<linestr.data;
			return out; 
		}
	};

	typedef std::istream_iterator<line> istream_line_iterator;
	typedef std::ostream_iterator<line> ostream_line_iterator;
}
#endif