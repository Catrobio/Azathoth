#include "Core.cpp"
using namespace std;

constexpr unsigned int str2int(const char* str, int h = 0)
{
	return !str[h] ? 5381 : (str2int(str, h + 1) * 33) ^ str[h];
} 

int main(int narg, char* argv[]) {
	Core Core;
	TormResult qry;
	bool tk;
	string TormResult;
	string valid, equ;
	string x;
	char* y;
	Function f;
	bool req;
	try
	{		
		printf("Content-type:application/json\n\n");
		for (int i = 1; i < narg; i++) {
			valid += argv[i];
			valid += " ";
		}
		valid = f.replaceAll(valid, "¡-", "(");
		valid = f.replaceAll(valid, "-¡", ")");		
		valid = f.replaceAll(valid, "!/", "\"");	
		vector<string> e = f.split(valid, "¿");
		tk = Core.GetToken(e[2]);
		x = e[0];		
		if (tk) 
		{
			x = e[0];
			y = (char *)x.c_str();
			switch (str2int(y))
			{
			case 207510041: //execG
				equ = f.replaceAll(e[1], "!S", "=");								
				//TormResult = "{\"Data\":[" + equ + "], \"CodeStatus\":\"-1111\", \"Msg\" : \"Testing\"}";
				qry = Core.GetQuery(equ);
				TormResult = "{\"Data\":[" + qry.Data + "], \"CodeStatus\":\"" + to_string(qry.CodeStatus) + "\", \"Msg\" : \"" + qry.Msg + "\"}";
				cout << TormResult;
				break;
			case 231218701: //execS
				equ = f.replaceAll(e[1], "!S", "=");
				qry = Core.ExecuteQuery(equ);
				TormResult = "{\"Data\":[\"" + qry.Data + "\"], \"CodeStatus\":\"" + to_string(qry.CodeStatus) + "\", \"Msg\" : \"" + qry.Msg + "\"}";
				cout << TormResult;
				break;		
			case  3857854400: //excToken				
				TormResult = "{\"Data\":[\"\"], \"CodeStatus\":\"1\", \"Msg\" : \"\"}";
				cout << TormResult;
				break;
			case  2755708178: //excRToken								
				cout << f.EncryptToken(f.Trim(e[2]));
				break;
			default:
				return 0;
				break;
			}
		}
		else {
			TormResult = "{\"Data\":[\"\"], \"CodeStatus\":\"-127\", \"Msg\" : \"Token no valido\"}";
			cout << TormResult;
		}
	}
	catch (std::exception const& e)
	{
		std::cerr << e.what() << std::endl;
	}
	return 0;
}