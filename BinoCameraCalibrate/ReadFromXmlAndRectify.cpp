#include"ReadFromXmlAndRectify.h"



int readParamsFromXml(const string& xml_path, const string& elem_name, vector<float>& params)
{
	XMLDocument doc;
	doc.LoadFile(xml_path.c_str());
	XMLElement* ptr_opencv_storage = doc.RootElement();
	if (!ptr_opencv_storage)
	{
		cout << "Read root node failed.\n";
		return -1;
	}

	XMLElement* ptr_rotate_left = ptr_opencv_storage->FirstChildElement(elem_name.c_str());
	if (ptr_rotate_left)
	{
		XMLElement* first_child = ptr_rotate_left->FirstChildElement();

		const char* content = "", *name = "";
		while (first_child)
		{
			name = first_child->Name();
			if (strcmp(name, "data") == 0)
			{
				content = first_child->GetText();
				break;
			}
			else
			{
				first_child = first_child->NextSiblingElement();
			}
		}
		//cout << "params:\n" << string(content) << endl;

		// return left camera distortion coefficients
		vector<string> tokens;
		splitStr(string(content), tokens, ' ');
		params.reserve(tokens.size());

		for (const auto& token : tokens)
		{
			//cout << token << endl;
			params.push_back((float)atof(token.c_str()));
		}
	}

	return 0;
}

int readLeftDistXml(const string& xml_path, vector<float>& l_dists)
{
	XMLDocument doc;
	doc.LoadFile(xml_path.c_str());
	XMLElement* ptr_opencv_storage = doc.RootElement();
	if (!ptr_opencv_storage)
	{
		cout << "Read root node failed.\n";
		return -1;
	}

	XMLElement* ptr_dist_left = ptr_opencv_storage->FirstChildElement("distCoeffL");
	if (ptr_dist_left)
	{
		XMLElement* first_child = ptr_dist_left->FirstChildElement();

		const char* content = "", *name = "";
		while (first_child)
		{
			name = first_child->Name();
			if (strcmp(name, "data") == 0)
			{
				content = first_child->GetText();
				break;
			}
			else
			{
				first_child = first_child->NextSiblingElement();
			}
		}
		cout << "Left distortion coefficients:\n" << string(content) << endl;

		// return left camera distortion coefficients
		l_dists.reserve(5);
		vector<string> tokens;
		splitStr(string(content), tokens, ' ');

		for (auto& token : tokens)
		{
			//cout << token << endl;
			l_dists.push_back((float)atof(token.c_str()));
		}
	}
	else
	{
		cout << "Read Node [distCoeffL] failed.\n";
		return -1;
	}

	return 0;
}


int readRightDistXml(const string& xml_path, vector<float>& r_dists)
{
	XMLDocument doc;
	doc.LoadFile(xml_path.c_str());
	XMLElement* ptr_opencv_storage = doc.RootElement();
	if (!ptr_opencv_storage)
	{
		cout << "Read root node failed.\n";
		return -1;
	}

	XMLElement* ptr_dist_right = ptr_opencv_storage->FirstChildElement("distCoeffR");
	if (ptr_dist_right)
	{
		XMLElement* first_child = ptr_dist_right->FirstChildElement();

		const char* content = "", *name = "";
		while (first_child)
		{
			name = first_child->Name();
			if (strcmp(name, "data") == 0)
			{
				content = first_child->GetText();
				break;
			}
			else
			{
				first_child = first_child->NextSiblingElement();
			}
		}
		cout << "Right distortion coefficients:\n" << string(content) << endl;

		// return right camera distortion coefficients
		r_dists.reserve(5);
		vector<string> tokens;
		splitStr(string(content), tokens, ' ');

		for (auto& token : tokens)
		{
			//cout << token << endl;
			r_dists.push_back((float)atof(token.c_str()));
		}
	}
	else
	{
		cout << "Read Node [distCoeffR] failed.\n";
		return -1;
	}

	return 0;
}


int readLeftRotateXml(const string& xml_path, vector<float>& l_rotate)
{
	XMLDocument doc;
	doc.LoadFile(xml_path.c_str());
	XMLElement* ptr_opencv_storage = doc.RootElement();
	if (!ptr_opencv_storage)
	{
		cout << "Read root node failed.\n";
		return -1;
	}

	XMLElement* ptr_rotate_left = ptr_opencv_storage->FirstChildElement("Rl");
	if (ptr_rotate_left)
	{
		XMLElement* first_child = ptr_rotate_left->FirstChildElement();

		const char* content = "", *name = "";
		while (first_child)
		{
			name = first_child->Name();
			if (strcmp(name, "data") == 0)
			{
				content = first_child->GetText();
				break;
			}
			else
			{
				first_child = first_child->NextSiblingElement();
			}
		}
		cout << "Right roration matrix:\n" << string(content) << endl;

		// return left camera distortion coefficients
		vector<string> tokens;
		splitStr(string(content), tokens, ' ');
		l_rotate.reserve(tokens.size());

		for (auto& token : tokens)
		{
			//cout << token << endl;
			l_rotate.push_back((float)atof(token.c_str()));
		}
	}

	return 0;
}




