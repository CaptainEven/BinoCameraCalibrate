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
	ret = string(src_str);  // string�Ŀ�������

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
	intptr_t hFile = 0;  // �ļ����  64λ��long ��Ϊ intptr_t
	struct _finddata_t file_info;  // �ļ���Ϣ 
	string p;
	if ((hFile = _findfirst(p.assign(path).append("/*").c_str(), &file_info)) != -1)  // �ļ��Ƿ����
	{
		do
		{
			if ((file_info.attrib & _A_SUBDIR))  // �ж��Ƿ�Ϊ�ļ���(Ŀ¼)
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
	intptr_t hFile = 0;  // �ļ����  64λ��long ��Ϊ intptr_t
	struct _finddata_t file_info;  // �ļ���Ϣ 
	string p;
	if ((hFile = _findfirst(p.assign(path).append("/*" + format).c_str(), &file_info)) != -1)  // �ļ�����
	{
		do
		{
			if ((file_info.attrib & _A_SUBDIR))  // �ж��Ƿ�Ϊ�ļ���
			{
				if (strcmp(file_info.name, ".") != 0 && strcmp(file_info.name, "..") != 0)  // �ļ������в���"."��".."
				{
					files.push_back(p.assign(path).append("/").append(file_info.name));  // �����ļ�����
					getFilesFormat(p.assign(path).append("/").append(file_info.name), format, files);  // �ݹ�����ļ���
				}
			}
			else
			{
				files.push_back(p.assign(path).append("/").append(file_info.name));  // ��������ļ��У������ļ���
			}
		} while (_findnext(hFile, &file_info) == 0);
		_findclose(hFile);
	}

	return int(files.size());
}
