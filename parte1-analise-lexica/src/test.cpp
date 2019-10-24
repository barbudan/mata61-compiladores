#include <bits/stdc++.h>

using namespace std;

#define TAMANHO_ARQUIVO 512001

////////////////////////////////////////////////////////////////
////////////////						////////////////////////
////////////////	 	ESTRUTURAS   	////////////////////////
////////////////						////////////////////////
////////////////////////////////////////////////////////////////

struct Token{
	string id;
	string data;
};

////////////////////////////////////////////////////////////////
////////////////						////////////////////////
////////////////		IMPRESSAO	   	////////////////////////
////////////////						////////////////////////
////////////////////////////////////////////////////////////////

void imprime_ok();
void imprime_invalido();
void imprime_erros_lexicos(list <pair<int,int>> &erros_lexicos);


////////////////////////////////////////////////////////////////
////////////////						////////////////////////
////////////////	 ANALISE LEXICA   	////////////////////////
////////////////						////////////////////////
////////////////////////////////////////////////////////////////

void create_reserved_words(list <string> &reservedwords);
void create_token(list <Token> &symbol_table, string id, string lexema);
void lex_estado_00(char buffer[], int &indice, list <pair<int,int>> &erros_lexicos, list <Token> &symbol_table, list <string> &reserved_words, int &linha, int &coluna, string &lexema, int &estado, int &achou_exclamacao);
void lex_estado_01(char buffer[], int &indice, list <pair<int,int>> &erros_lexicos, list <Token> &symbol_table, list <string> &reserved_words, int &linha, int &coluna, string &lexema, int &estado);
void lex_estado_02(char buffer[], int &indice, list <pair<int,int>> &erros_lexicos, list <Token> &symbol_table, list <string> &reserved_words, int &linha, int &coluna, string &lexema, int &estado, int &achou_ponto);
void lex_estado_03(char buffer[], int &indice, list <pair<int,int>> &erros_lexicos, list <Token> &symbol_table, list <string> &reserved_words, int &linha, int &coluna, string &lexema, int &estado, int &achou_exclamacao);
void lex_estado_04(char buffer[], int &indice, list <pair<int,int>> &erros_lexicos, list <Token> &symbol_table, list <string> &reserved_words, int &linha, int &coluna, string &lexema, int &estado, int &guarda_char_nl);
void lex_estado_05(char buffer[], int &indice, list <pair<int,int>> &erros_lexicos, list <Token> &symbol_table, list <string> &reserved_words, int &linha, int &coluna, string &lexema, int &estado, int &guarda_char_nl);


////////////////////////////////////////////////////////////////
////////////////						////////////////////////
////////////////	 VERIFICADORES   	////////////////////////
////////////////						////////////////////////
////////////////////////////////////////////////////////////////

bool is_valid(char c);
bool is_eof(char c);
bool is_letra(char c);
bool is_underscore(char c);
bool is_number(char c);
bool is_special_char(char c);
bool is_separador(char c);
bool is_reserved_word(string lexema, list <string> &reserved_words);


////////////////////////////////////////////////////////////////
////////////////						////////////////////////
////////////////	PROGRAMA PRINCIPAL	////////////////////////
////////////////						////////////////////////
////////////////////////////////////////////////////////////////

int main() {

 	///////////////////////////
	//////// LEITURA //////////
	///////////////////////////

	long long int tamanho_entrada;
	char buffer[512001];
	tamanho_entrada = fread(&buffer,1,TAMANHO_ARQUIVO,stdin);
	buffer[tamanho_entrada] = '\0';


	///////////////////////////
	///// ANALISE LEXICA //////
	///////////////////////////

	int coluna=1,linha=1;
	string lexema;
	lexema.clear();
	int indice = 0;
	int estado = 0;
	int achou_ponto = 0;
	int achou_exclamacao = 0;
	int guarda_char_nl = 0;
	list <pair<int, int>> erros_lexicos;

	list <string> reserved_words;
	create_reserved_words(reserved_words);

	list <Token> symbol_table;

	while(!is_eof(buffer[indice]))
	{
		if(!is_valid(buffer[indice]))
		{
			imprime_invalido();
			return 0;
		}
		
		switch(estado)
		{
			case 0:
				lex_estado_00(buffer, indice, erros_lexicos, symbol_table, reserved_words, linha, coluna, lexema, estado, achou_exclamacao);
				break;
			case 1:
				lex_estado_01(buffer, indice, erros_lexicos, symbol_table, reserved_words, linha, coluna, lexema, estado);
				break;
			case 2:
				lex_estado_02(buffer, indice, erros_lexicos, symbol_table, reserved_words, linha, coluna, lexema, estado, achou_ponto);
				break;
			case 3:
				lex_estado_03(buffer, indice, erros_lexicos, symbol_table, reserved_words, linha, coluna, lexema, estado, achou_exclamacao);
				break;
			case 4:
				lex_estado_04(buffer, indice, erros_lexicos, symbol_table, reserved_words, linha, coluna, lexema, estado, guarda_char_nl);
				break;
			case 5:
				lex_estado_05(buffer, indice, erros_lexicos, symbol_table, reserved_words, linha, coluna, lexema, estado, guarda_char_nl);
				break;
		}
		indice++;
		coluna++;
	}

	if(erros_lexicos.empty())
		imprime_ok();
	else
		imprime_erros_lexicos(erros_lexicos);

	list <Token>::iterator it;

/*	for(it=symbol_table.begin();it!=symbol_table.end();it++)
		cout << (*it).id << " " << (*it).data << endl;
*/

	///////////////////////////
	//// ANALISE SINTATICA ////
	///////////////////////////
	
	return 0;
}

////////////////////////////////////////////////////////////////
////////////////						////////////////////////
////////////////		IMPRESSAO	   	////////////////////////
////////////////						////////////////////////
////////////////////////////////////////////////////////////////

void imprime_ok(){
	cout << "OK\n";
}

void imprime_invalido(){
	cout << "ARQUIVO INVALIDO\n";
}

void imprime_erros_lexicos(list <pair<int,int>> &erros_lexicos){
	list <pair<int,int>>::iterator it;

	for(it=erros_lexicos.begin();it!=erros_lexicos.end();it++)
		cout << (*it).first << " " << (*it).second << endl;
}


////////////////////////////////////////////////////////////////
////////////////						////////////////////////
////////////////	 VERIFICADORES   	////////////////////////
////////////////						////////////////////////
////////////////////////////////////////////////////////////////


bool is_letra(char c){
	if((c>='a' && c<='z') || (c>='A' && c<='Z'))
		return true;
	return false;
}

bool is_underscore(char c){
	if(c=='_')
		return true;
	return false;
}

bool is_number(char c){
	if(c>='0' && c<='9')
		return true;
	return false;
}

bool is_valid(char c){
	if((c>=32 && c<=126) || c==9 || c==10)
		return true;
	return false;
}

bool is_eof(char c){
	if(c=='\0')
		return true;
	return false;
}

bool is_special_char(char c){
	if(c=='#' || c=='$' || c=='@' || c=='&' || c=='~' || c=='|' || c=='^' || c=='`' || c==92 || c=='?' || c==':' || c=='.')
		return true;
	return false;
}

bool is_separador(char c){
	if(c=='\n' || c==' ' || c=='\t')
		return true;
	return false;
}

bool is_reserved_word(string lexema, list <string> &reserved_words){
	list <string>::iterator it;
	for(it=reserved_words.begin();it!=reserved_words.end();it++)
	{
		if((*it)==lexema)
			return true;
	}
	return false;
}


////////////////////////////////////////////////////////////////
////////////////						////////////////////////
////////////////	 ANALISE LEXICA   	////////////////////////
////////////////						////////////////////////
////////////////////////////////////////////////////////////////

void create_reserved_words(list <string> &reservedwords){
	reservedwords.push_back("se");
	reservedwords.push_back("senao");
	reservedwords.push_back("e");
	reservedwords.push_back("ou");
	reservedwords.push_back("le");
	reservedwords.push_back("escreve");
	reservedwords.push_back("int");
	reservedwords.push_back("letra");
	reservedwords.push_back("real");
	reservedwords.push_back("mainha");
	reservedwords.push_back("nada");
	reservedwords.push_back("nao");
	reservedwords.push_back("tome");
	reservedwords.push_back("uai");
	reservedwords.push_back("vaza");
}

void create_token(list <Token> &symbol_table, string id, string lexema){
	Token tk;
	tk.id = id;
	tk.data = lexema;
	symbol_table.push_back(tk);
}

void lex_estado_00(char buffer[], int &indice, list <pair<int,int>> &erros_lexicos, list <Token> &symbol_table, list <string> &reserved_words, int &linha, int &coluna, string &lexema, int &estado, int &achou_exclamacao){
	if(is_letra(buffer[indice]) || is_underscore(buffer[indice]))
	{
		estado=1;
		lexema+=buffer[indice];
		if(is_eof(buffer[indice+1]))
		{
			if(is_reserved_word(lexema, reserved_words))
				create_token(symbol_table, lexema, lexema);
			else
				create_token(symbol_table, "ID", lexema);

			lexema.clear();
		}
	}
	else if(is_number(buffer[indice]))
	{
		estado=2;
		lexema+=buffer[indice];
		if(is_eof(buffer[indice+1]))
		{
			create_token(symbol_table, "NUMBER", lexema);
			lexema.clear();
		}				
	}
	else if(buffer[indice]=='<' || buffer[indice]=='>' || buffer[indice]=='=' || buffer[indice]=='!')
	{
		if(buffer[indice]=='!')
			achou_exclamacao=1;

		estado=3;
		lexema+=buffer[indice];

		if(is_eof(buffer[indice+1]))
		{
			if(buffer[indice]=='!')
				erros_lexicos.push_back(make_pair(linha,coluna));
			else
			{
				lexema = buffer[indice];
				if(buffer[indice]=='=')
					create_token(symbol_table, "ATRIBUICAO", lexema);
				else
					create_token(symbol_table, "COMPARADOR", lexema);
			}
			lexema.clear();
		}
	}
	else if(buffer[indice]==39)
	{
		estado=4;
		lexema+=buffer[indice];

		if(is_eof(buffer[indice+1]))
		{
			erros_lexicos.push_back(make_pair(linha,coluna));
			lexema.clear();
		}
	}
	else if(buffer[indice]==34)
	{
		estado=5;
		lexema+=buffer[indice];

		if(is_eof(buffer[indice+1]))
		{
			erros_lexicos.push_back(make_pair(linha,coluna));
			lexema.clear();
		}	
	}
	else if(is_special_char(buffer[indice]))
	{
		erros_lexicos.push_back(make_pair(linha, coluna));
		lexema.clear();
	}
	else if(buffer[indice]=='\n')
	{
		linha++;
		coluna=0;
	}				
	else if(!is_separador(buffer[indice]))
	{
		lexema = buffer[indice];
		create_token(symbol_table, lexema, lexema);
		lexema.clear();			
	}
}

void lex_estado_01(char buffer[], int &indice, list <pair<int,int>> &erros_lexicos, list <Token> &symbol_table, list <string> &reserved_words, int &linha, int &coluna, string &lexema, int &estado){
	if(is_letra(buffer[indice]) || is_underscore(buffer[indice]) || is_number(buffer[indice]))
	{
		if(lexema.size()!=128)
		{
			lexema+=buffer[indice];
			if(is_eof(buffer[indice+1]))
			{
				if(is_reserved_word(lexema, reserved_words))
					create_token(symbol_table, lexema, lexema);
				else
					create_token(symbol_table, "ID", lexema);

				lexema.clear();
			}
		}
		else
		{
			estado=0;
			erros_lexicos.push_back(make_pair(linha,coluna-1));
			indice--;
			coluna--;
			lexema.clear();
		}
	}
	else if(buffer[indice]==' ' || buffer[indice]=='\t')
	{
		estado=0;
		if(is_reserved_word(lexema, reserved_words))
			create_token(symbol_table, lexema, lexema);
		else
			create_token(symbol_table, "ID", lexema);

		lexema.clear();
	}
	else if(buffer[indice]=='\n')
	{
		estado=0;
		if(is_reserved_word(lexema, reserved_words))
			create_token(symbol_table, lexema, lexema);
		else
			create_token(symbol_table, "ID", lexema);
		lexema.clear();
		linha++;
		coluna=0;
	}
	else
	{
		estado=0;
		if(is_reserved_word(lexema, reserved_words))
			create_token(symbol_table, lexema, lexema);
		else
			create_token(symbol_table, "ID", lexema);
		lexema.clear();
		indice--;
		coluna--;
	}
}

void lex_estado_02(char buffer[], int &indice, list <pair<int,int>> &erros_lexicos, list <Token> &symbol_table, list <string> &reserved_words, int &linha, int &coluna, string &lexema, int &estado, int &achou_ponto){
	if(is_number(buffer[indice]))
	{
		if(lexema.size()!=128)
		{
			lexema+=(buffer[indice]);
			if(is_eof(buffer[indice+1]))
			{
				create_token(symbol_table, "NUMBER", lexema);
				lexema.clear();
			}
		}
		else
		{
			estado=0;
			erros_lexicos.push_back(make_pair(linha,coluna-1));
			indice--;
			coluna--;
			lexema.clear();
			achou_ponto=0;
		}
	}
	else if(buffer[indice]==' ' || buffer[indice]=='\t')
	{
		estado=0;
		create_token(symbol_table, "NUMBER", lexema);
		lexema.clear();
		achou_ponto=0;
	}
	else if(buffer[indice]=='\n')
	{
		estado=0;
		create_token(symbol_table, "NUMBER", lexema);
		lexema.clear();
		achou_ponto=0;
		linha++;
		coluna=0;
	}
	else if(is_letra(buffer[indice]) || is_underscore(buffer[indice]))
	{
		estado=0;
		erros_lexicos.push_back(make_pair(linha,coluna-1));
		lexema.clear();
		achou_ponto=0;
		indice--;
		coluna--;
	}
	else if(buffer[indice]=='.')
	{
		if(achou_ponto)
		{
			estado=0;
			erros_lexicos.push_back(make_pair(linha,coluna-1));
			erros_lexicos.push_back(make_pair(linha,coluna));
			lexema.clear();
			achou_ponto=0;
		}
		else
		{
			if(lexema.size()!=128)
			{
				lexema+=(buffer[indice]);
				achou_ponto=1;
				if(is_eof(buffer[indice+1]))
				{
					create_token(symbol_table, "NUMBER", lexema);
					lexema.clear();
				}
			}
			else
			{
				estado=0;
				erros_lexicos.push_back(make_pair(linha,coluna-1));
				lexema.clear();
				achou_ponto=0;
				indice--;
				coluna--;
			}
		}
	}
	else
	{
		estado=0;
		create_token(symbol_table, "NUMBER", lexema);
		lexema.clear();
		achou_ponto=0;
		indice--;
		coluna--;
	}
}

void lex_estado_03(char buffer[], int &indice, list <pair<int,int>> &erros_lexicos, list <Token> &symbol_table, list <string> &reserved_words, int &linha, int &coluna, string &lexema, int &estado, int &achou_exclamacao){
	if(buffer[indice]=='=')
	{
		estado=0;
		lexema+=(buffer[indice]);
		create_token(symbol_table,"COMPARADOR",lexema);
		lexema.clear();
		achou_exclamacao=0;
	}
	else if(buffer[indice]==' ' || buffer[indice]=='\t' || buffer[indice]=='\n')
	{
		if(!achou_exclamacao)
		{
			estado=0;
			create_token(symbol_table,"COMPARADOR",lexema);
			lexema.clear();
			if(buffer[indice]=='\n')
			{
				linha++;
				coluna=0;
			}
		}
		else
		{
			estado=0;
			erros_lexicos.push_back(make_pair(linha,coluna-1));
			lexema.clear();
			indice--;					
			coluna--;
			achou_exclamacao=0;
		}
	}
	else if(achou_exclamacao)
	{
		estado=0;
		erros_lexicos.push_back(make_pair(linha,coluna-1));
		lexema.clear();
		indice--;					
		coluna--;
		achou_exclamacao=0;
	}
	else
	{
		estado=0;
		create_token(symbol_table,"COMPARADOR",lexema);
		lexema.clear();
		indice--;
		coluna--;
		achou_exclamacao=0;
	}
}

void lex_estado_04(char buffer[], int &indice, list <pair<int,int>> &erros_lexicos, list <Token> &symbol_table, list <string> &reserved_words, int &linha, int &coluna, string &lexema, int &estado, int &guarda_char_nl){
	if(lexema.size()==1)
	{
		if(buffer[indice]==39)
		{
			estado=0;
			erros_lexicos.push_back(make_pair(linha,coluna-1));
			lexema.clear();
			indice--;
			coluna--;						
		}
		else if(buffer[indice]=='\n')
		{
			lexema+=(buffer[indice]);
			linha++;
			guarda_char_nl=coluna;
			coluna=0;

			if(is_eof(buffer[indice+1]))
			{
				erros_lexicos.push_back(make_pair(linha-1,guarda_char_nl));
				lexema.clear();
			}
		}
		else
		{
			lexema+=(buffer[indice]);
			if(is_eof(buffer[indice+1]))
			{
				erros_lexicos.push_back(make_pair(linha,coluna));
				lexema.clear();
			}			
		}
	}
	else if(lexema.size()==2)
	{
		if(buffer[indice]==39)
		{
			estado=0;
			lexema+=buffer[indice];
			create_token(symbol_table,"CONST_CHAR",lexema);
			lexema.clear();
		}
		else
		{
			estado=0;
			if(lexema.back()=='\n') erros_lexicos.push_back(make_pair(linha-1,guarda_char_nl));
			else erros_lexicos.push_back(make_pair(linha,coluna-1));
			lexema.clear();
			indice--;
			coluna--;
		}
	}				
}

void lex_estado_05(char buffer[], int &indice, list <pair<int,int>> &erros_lexicos, list <Token> &symbol_table, list <string> &reserved_words, int &linha, int &coluna, string &lexema, int &estado, int &guarda_char_nl){
	if(lexema.size()!=129)
	{
		if(buffer[indice]=='"')
		{
			estado=0;
			lexema+=buffer[indice];
			create_token(symbol_table,"CONST_STRING",lexema);
			lexema.clear();
		}
		else if(buffer[indice]=='\n')
		{
			lexema+=(buffer[indice]);
			linha++;
			guarda_char_nl=coluna;
			coluna=0;

			if(is_eof(buffer[indice+1]))
			{
				erros_lexicos.push_back(make_pair(linha-1,guarda_char_nl));
				lexema.clear();
			}
		}
		else
		{
			if(is_eof(buffer[indice+1]))
			{
				erros_lexicos.push_back(make_pair(linha,coluna));
				lexema.clear();
			}
			lexema+=(buffer[indice]);
		}
	}
	else
	{
		if(buffer[indice]=='"')
		{
			estado=0;
			create_token(symbol_table,"CONST_STRING",lexema);
			lexema.clear();
		}
		else
		{
			estado=0;
			if(lexema.back()=='\n')erros_lexicos.push_back(make_pair(linha-1,guarda_char_nl));
			else
			{
				if(coluna==0) coluna=2;
				erros_lexicos.push_back(make_pair(linha,coluna-1));
			}
			lexema.clear();
			indice--;
			coluna--;
		}
	}
}

////////////////////////////////////////////////////////////////
////////////////						////////////////////////
////////////////    ANALISE SINTATICA   ////////////////////////
////////////////						////////////////////////
////////////////////////////////////////////////////////////////