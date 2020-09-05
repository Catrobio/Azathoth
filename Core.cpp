#include <algorithm>
#include <fstream>
#include <ctime>
#include <string>
#include <../../postgresql/libpq-fe.h>
#include "TormResult.cpp"
#include "Functions.cpp"
using namespace std;

class Core
{
private:
	PGconn *cnn = NULL;
	PGresult *result = NULL;
	char *host;
	char *port;
	char *dataBase;
	char *user;
	char *passwd;
	char *Qry;
	Function f;
	string err, strHost, strPort, strDataBase, strSchema, strUser, strPassword;
	bool Connect()
	{
		string line;
		int con = 0;
		ifstream fileconf("/Conf.lvc");
		fileconf.open("Conf.lvc");
		if (fileconf.is_open())
		{
			while (!fileconf.eof())
			{
				getline(fileconf, line);
				if (con == 0)
				{
					strHost = f.Descrypt(line);
				}
				if (con == 1)
				{
					strPort = f.Descrypt(line);
				}
				if (con == 2)
				{
					strDataBase = f.Descrypt(line);
				}
				if (con == 3)
				{
					strSchema = f.Descrypt(line);
				}
				if (con == 4)
				{
					strUser = f.Descrypt(line);
				}
				if (con == 5)
				{
					strPassword = f.Descrypt(line);
				}
				con++;
			}
			host = (char *)strHost.c_str();
			port = (char *)strPort.c_str();
			dataBase = (char *)strDataBase.c_str();
			user = (char *)strUser.c_str();
			passwd = (char *)strPassword.c_str();
			cnn = PQsetdbLogin(host, port, NULL, NULL, dataBase, user, passwd);
			if (PQstatus(cnn) != CONNECTION_BAD)				
				return true;
			else
			{
				err =   f.Trim(f.replaceAll(f.replaceAll(f.replaceAll(PQerrorMessage(cnn),"\n"," "),"\""," "),"^",""));
				return false;
			}				
		}
		else
		{
			perror("Conf.lvc");
			return false;
		}
	}

	bool Disconnect()
	{
		PQclear(result);
		PQfinish(cnn);
		return true;
	}

	bool ValidQuery(string sql)
	{
		int tf = 0;
		if (f.Isreplace(sql, "ALTER", ""))
			tf += 1;
		if (f.Isreplace(sql, "DROP", ""))
			tf += 1;
		if (f.Isreplace(sql, "TRUNCATE", ""))
			tf += 1;
		if (f.Isreplace(sql, "LOCKET", ""))
			tf += 1;
		if (f.Isreplace(sql, "TABLE", ""))
			tf += 1;
		if (f.Isreplace(sql, "SHOW", ""))
			tf += 1;
		if (f.Isreplace(sql, "DATABASE", ""))
			tf += 1;
		if (f.Isreplace(sql, "DECLARE", ""))
			tf += 1;
		if (tf == 0)
		{
			return true;
		}
		else
		{
			return false;
		}
	}

	bool ValidQuerySelect(string sql)
	{
		int tf = 0;
		sql = f.UpperString(sql);
		if (ValidQuery(sql))
		{
			if (f.Isreplace(sql, "INSERT", ""))
				tf += 1;
			if (f.Isreplace(sql, "UPDATE", ""))
				tf += 1;
			if (f.Isreplace(sql, "DELETE", ""))
				tf += 1;
		}
		else
		{
			return false;
		}
		if (tf == 0)
		{
			return true;
		}
		else
		{
			return false;
		}
	}

	bool ValidQueryDelete(string sql)
	{
		int tf = 0;
		if (ValidQuery(sql))
		{
			sql = f.UpperString(sql);
			if (f.Isreplace(sql, "INSERT", ""))
				tf += 1;
			if (f.Isreplace(sql, "UPDATE", ""))
				tf += 1;
		}
		if (tf == 0)
		{
			return true;
		}
		else
		{
			return false;
		}
	}

	bool ValidQueryUpdate(string sql)
	{
		int tf = 0;
		if (ValidQuery(sql))
		{
			sql = f.UpperString(sql);
			if (f.Isreplace(sql, "INSERT", ""))
				tf += 1;
			if (f.Isreplace(sql, "DELETE", ""))
				tf += 1;
		}
		if (tf == 0)
		{
			return true;
		}
		else
		{
			return false;
		}
	}

	bool ValidQueryInsert(string sql)
	{
		int tf = 0;
		if (ValidQuery(sql))
		{
			sql = f.UpperString(sql);
			if (f.Isreplace(sql, "UPDATE", ""))
				tf += 1;
			if (f.Isreplace(sql, "DELETE", ""))
				tf += 1;
		}
		if (tf == 0)
		{
			return true;
		}
		else
		{
			return false;
		}
	}

	TormResult S_Query(string qry)
	{
		TormResult Res;
		Res.CodeStatus = 0;
		Res.Msg = "";
		if (ValidQuery(qry))
		{
			qry = f.replace(qry, "\\", "");
			if (Connect())
			{
				try
				{
					Qry = (char *)qry.c_str();
					result = PQexec(cnn, Qry);
					if (PQresultStatus(result) == PGRES_COMMAND_OK)
					{
						int rows = PQntuples(result);
						int fields = PQnfields(result);
						Res.Msg = "{}";
						Res.CodeStatus = 1;
					}
					else
					{
						err =   f.Trim(f.replaceAll(f.replaceAll(f.replaceAll(PQerrorMessage(cnn),"\n"," "),"\""," "),"^",""));
						Res.Msg = "Query incorrecto! " + err;
						Res.CodeStatus = -700;
					}
					Disconnect();
				}
				catch (std::exception const &e)
				{
					Res.CodeStatus = -1;
					Res.Msg = " Error al ejecutar la operacion";
					Disconnect();
				}
			}
			else
			{
				Res.CodeStatus = -101;
				Res.Msg = "No hay conexion con la base de datos";
			}
		}
		else
		{
			Res.CodeStatus = -211;
			Res.Msg = "Sql no valido!";
		}
		return Res;
	}

	bool SS_Query(string qry)
	{
		if (ValidQueryInsert(qry))
		{
			qry = f.replace(qry, "\\", "");
			if (Connect())
			{
				try
				{
					string Tran;
					Tran = qry;
					Qry = (char *)Tran.c_str();
					result = PQexec(cnn, Qry);
					if (result != NULL)
					{
						return true;
					}
					else
					{
						return false;
					}
				}
				catch (exception &e)
				{
					return false;
				}
			}
			else
			{
				return false;
			}
		}
		else
		{
			return false;
		}
	}

	TormResult G_Query(string qry)
	{
		TormResult Res;
		Res.CodeStatus = 0;
		Res.Msg = "";
		Res.Data = "";
		if (ValidQuerySelect(qry))
		{
			qry = f.replace(qry, "\\", "");
			if (Connect())
			{
				try
				{
					Qry = (char *)qry.c_str();
					result = PQexec(cnn, Qry);
					if (PQresultStatus(result) == PGRES_TUPLES_OK)
					{
						int rows = PQntuples(result);
						int fields = PQnfields(result);
						if (fields != 0 && rows != 0)
						{
							for (int j = 0; j < rows; j++)
							{
								Res.Data = Res.Data + "{";
								for (int i = 0; i < fields; i++)
								{
									Res.Data = Res.Data + "\"" + PQfname(result, i) + "\"" + ":";
									Res.Data = Res.Data + "\"" + PQgetvalue(result, j, i) + "\"" + ",";
								}
								Res.Data = f.RTrim(Res.Data, ",");
								Res.Data = Res.Data + "},";
							}
							Res.Data = f.RTrim(Res.Data, ",");
							Res.CodeStatus = 1;
						}
						else
						{
							Res.CodeStatus = 1;
							Res.Msg = "No Rows";
						}
					}
					else
					{
						err =  f.Trim(f.replaceAll(f.replaceAll(f.replaceAll(PQerrorMessage(cnn),"\n"," "),"\""," "),"^",""));
						Res.CodeStatus = -101;
						Res.Msg = "Error en consulta. "+err;
					}
					Disconnect();
					return Res;
				}
				catch (std::exception const &e)
				{
					Res.CodeStatus = -1;
					Res.Msg = " Error al ejecutar la operacion";
					Disconnect();
				}
			}
			else
			{
				Res.CodeStatus = -101;
				Res.Msg = "No hay conexion con la base de datos";
				Disconnect();
			}
		}
		else
		{
			Res.CodeStatus = -111;
			Res.Msg = "Sql no valido!";
		}
		return Res;
	}

public:
	TormResult ExecuteQuery(string qry)
	{
		return S_Query(qry);
	}

	TormResult GetQuery(string qry)
	{
		return G_Query(qry);
	}

	bool GetToken(string toke)
	{
		bool istoken = false;
		try
		{
			Connect();
			toke = f.replace(toke, "\\", "");
			string qry = "Select UserToken from  \"" + strSchema + "\".\"lvc_userauthentication\" a inner join  \"" + strSchema + "\".\"lvc_users\" b on b.IdUser = a.User_IdUser where UserToken='" + f.EncryptToken(f.Trim(toke)) + "' AND NOW()<= DateExpiration AND IsActive = 1 AND b.Iduserstatus = 1";
			Disconnect();
			TormResult result = G_Query(qry);
			if (result.CodeStatus == 1)
			{
				if (result.Data != "" && result.Data != "{}")
					istoken = true;
			}
		}
		catch (const std::exception &e)
		{
			istoken = false;
			return istoken;
		}
		return istoken;
	}
};