#include"utils.h"


void splitStr(const string& s, vector<string>& tokens, const char& delim = ' ')
{
	tokens.clear();
	size_t last_pos = s.find_first_not_of(delim, 0);
	size_t pos = s.find(delim, last_pos);
	while (last_pos != string::npos)
	{
		tokens.emplace_back(s.substr(last_pos, pos - last_pos));
		last_pos = s.find_first_not_of(delim, pos);
		pos = s.find(delim, last_pos);
	}
}


void replaceStr(const string& src_str,
	const string &old_str, const string& new_str,
	string& ret,
	int count = -1)
{
	ret = string(src_str);  // string的拷贝构造

	size_t pos = 0;
	int l_count = 0;
	if (-1 == count)  // replace all
	{
		count = (int)ret.size();  // max size
	}
	while ((pos = ret.find(old_str, pos)) != string::npos)
	{
		ret.replace(pos, old_str.size(), new_str);
		if (++l_count >= count)
		{
			break;
		}

		pos += new_str.size();
	}
}


const int getDirs(const string & path, vector<string>& dirs)
{
	intptr_t hFile = 0;  // 文件句柄  64位下long 改为 intptr_t
	struct _finddata_t file_info;  // 文件信息 
	string p;
	if ((hFile = _findfirst(p.assign(path).append("/*").c_str(), &file_info)) != -1)  // 文件是否存在
	{
		do
		{
			if ((file_info.attrib & _A_SUBDIR))  // 判断是否为文件夹(目录)
			{
				if (strcmp(file_info.name, ".") != 0 && strcmp(file_info.name, "..") != 0)
				{
					dirs.push_back(p.assign(path).append("/").append(file_info.name));
				}
			}
		} while (_findnext(hFile, &file_info) == 0);
		_findclose(hFile);
	}

	return int(dirs.size());
}


const int getFilesFormat(const string& path, const string& format, vector<string>& files)
{
	intptr_t hFile = 0;  // 文件句柄  64位下long 改为 intptr_t
	struct _finddata_t file_info;  // 文件信息 
	string p;
	if ((hFile = _findfirst(p.assign(path).append("/*" + format).c_str(), &file_info)) != -1)  // 文件存在
	{
		do
		{
			if ((file_info.attrib & _A_SUBDIR))  // 判断是否为文件夹
			{
				if (strcmp(file_info.name, ".") != 0 && strcmp(file_info.name, "..") != 0)  // 文件夹名中不含"."和".."
				{
					files.push_back(p.assign(path).append("/").append(file_info.name));  // 保存文件夹名
					getFilesFormat(p.assign(path).append("/").append(file_info.name), format, files);  // 递归遍历文件夹
				}
			}
			else
			{
				files.push_back(p.assign(path).append("/").append(file_info.name));  // 如果不是文件夹，储存文件名
			}
		} while (_findnext(hFile, &file_info) == 0);
		_findclose(hFile);
	}

	return int(files.size());
}
