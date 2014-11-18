#ifndef DCEL_STLPACHE_FILE_H
#define DCEL_STLPACHE_FILE_H

#include <windows.h>
#include <iterator>
#include <string>

class win32_file_iterator : public std::iterator<std::input_iterator_tag, std::string>
{
private:
	class internal_handle_data{
	public:
		internal_handle_data():_h(NULL), _ref(0){}
		void setHandle(HANDLE handle){ _h = handle; }
		HANDLE getHandle(){ return _h; }
		void incRef(){ _ref++; }
		unsigned decRef(){ return --_ref; }
		operator HANDLE(){ return _h; }

	private:
		HANDLE _h;
		unsigned _ref;
	};
public:
	win32_file_iterator(std::string strfilter, bool bFullPath = false, 
		int flag = FILE_ATTRIBUTE_ARCHIVE|FILE_ATTRIBUTE_NORMAL|FILE_ATTRIBUTE_DIRECTORY):
	_bEnd(false), _bFullPath(bFullPath), _flag(flag){
		HANDLE h = ::FindFirstFile(strfilter.c_str(), &_wfd);
		_handle.setHandle(h);
		if(h == INVALID_HANDLE_VALUE){
			_bEnd = true;
		}else{
			_handle.incRef();
			std::string::size_type n1 = strfilter.find_last_of("\\");
			_strroot = strfilter.substr(0,n1+1);
			_chkvalid(_wfd);
		}
	}
	win32_file_iterator():_bEnd(true){}
	win32_file_iterator(win32_file_iterator& rhs){

		_handle = rhs._handle;
		_handle.incRef();
		_flag = rhs._flag;
		_bFullPath = rhs._bFullPath;
		_bEnd = rhs._bEnd;
		_wfd = rhs._wfd;
		_strfname = rhs._strfname;
		_strroot = rhs._strroot;  
	}
	~win32_file_iterator(){
		if(_handle.decRef() == 0 && _handle.getHandle() != NULL ){
			FindClose(_handle);
		}

	}
	reference operator*(){
		return _strfname;
	}
	bool operator==(const win32_file_iterator& rhs) const{
		return (_bEnd == rhs._bEnd);
	}
	bool operator!=(const win32_file_iterator& rhs) const{
		return (_bEnd != rhs._bEnd);
	}
	win32_file_iterator& operator++(){
		_findnext();
		return *this;
	}
	win32_file_iterator& operator++(int){
		_findnext();
		return *this;
	}

private:
	void _findnext(){
		BOOL b = ::FindNextFile(_handle, &_wfd);
		if(b){
			_chkvalid(_wfd);
		}else{
			_bEnd = true;
		}
	}
	void _chkvalid(WIN32_FIND_DATA& _wfd){
		if(_wfd.dwFileAttributes & _flag){
			_getval(_wfd);
		}
		else{
			_findnext();
		}
	}
	void _getval(WIN32_FIND_DATA& wfd){
		if(_bFullPath)
			_strfname = _strroot + wfd.cFileName;
		else
			_strfname = wfd.cFileName;
	}
private:
	int _flag;
	bool _bFullPath;
	bool _bEnd;
	internal_handle_data _handle;
	WIN32_FIND_DATA _wfd;
	std::string _strroot;
	std::string _strfname;
};

#endif