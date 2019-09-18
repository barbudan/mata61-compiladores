#include <bits/stdc++.h>

using namespace std;

#define TAMANHO_ARQUIVO 512001

int redecl_var=0;
int redecl_func=0;
int func_nao_declarada=0;
int var_nao_declarada=0;
int conflito_var_func=0;
set <string> lista_funcs;
set <string> lista_vars;
int porra=0;

////////////////////////////////////////////////////////////////
////////////////						////////////////////////
////////////////	 	ESTRUTURAS   	////////////////////////
////////////////						////////////////////////
////////////////////////////////////////////////////////////////

struct Token{
	string id;
	string data;
};

struct Node{ // Arvore Semantica
	Token token;
	Node *pai;
	vector <Node *> filhos;
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
bool is_comparador(string lexema);
bool is_operador_arit(char c);
bool is_shift(string s);
bool is_reduce(string s);
bool is_transition(string s);


////////////////////////////////////////////////////////////////
////////////////						////////////////////////
////////////////	   AUXILIARES   	////////////////////////
////////////////						////////////////////////
////////////////////////////////////////////////////////////////

int to_int(string s);


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
////////////////   ANALISE SINTATICA 	////////////////////////
////////////////						////////////////////////
////////////////////////////////////////////////////////////////

void preenche_tabela(map <pair<int,string>,string> &tabela);
void preenche_transicoes(map <int, pair<int,string>> &transicoes);


////////////////////////////////////////////////////////////////
////////////////						////////////////////////
//////////////// 	ANALISE SEMANTICA 	////////////////////////
////////////////						////////////////////////
////////////////////////////////////////////////////////////////

Node *criaRaiz(Token tk);
Node *newNode(Token tk, Node *pai);
Node *newNodeSemPai(Token tk);
void imprimeArvore(Node *raiz);
void imprimeArvoreDFS(Node *raiz);
void preencheCabecaRegra(set <string> &regras);
void declaracao_func(Node *raiz);
void declaracao_vars_em_param(Node *raiz);
void declaracao_vars(Node *raiz);
void executa_bloco(Node *raiz);
void percorreArvoreDFS(Node *raiz);
void testa_atribuicao(Node *raiz);


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
			//imprime_invalido();
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

	/*if(erros_lexicos.empty())
		imprime_ok();
	else
		imprime_erros_lexicos(erros_lexicos);
	*/
	list <Token>::iterator it;

	/*for(it=symbol_table.begin();it!=symbol_table.end();it++)
		cout << (*it).id << " " << (*it).data << endl;
	*/

	///////////////////////////
	//// ANALISE SINTATICA ////
	///////////////////////////

	stack <pair<int,Token>> pilha;
	map <pair<int,string>,string> tabela;
	map <int, pair<int,string>> transicoes;
	pair <int,Token> topo;
	
	preenche_transicoes(transicoes); // preenche quantidade de elementos em cada transicao
	preenche_tabela(tabela); // preenche a tabela da gramatica

	Token aux_tk;
	aux_tk.id = "";
	aux_tk.data = "";

	it = symbol_table.begin();
	pilha.push(make_pair(0,aux_tk)); // estado 0 adicionado na pilha
	
	aux_tk.id = "$";
	aux_tk.data = "$";
	symbol_table.push_back(aux_tk);

	aux_tk.id = "";

	topo = pilha.top();
	/*if(tabela.find(make_pair(topo.first, (*it).id) ) != tabela.end())
		cout << tabela[make_pair(topo.first, (*it).id)] << endl;
	*/

	list <pair<int,Token>> teste; // lista teste para imprimir pilha


	int erro=0;
	int operacao = 1;


	// SEMANTICO //
	Node *no;
	set <string> regras;
	preencheCabecaRegra(regras);

	stack <Node*> semantic_stack;

	while(!pilha.empty())
	{
		topo = pilha.top();

		if(tabela.find(make_pair(topo.first, (*it).id)) != tabela.end() && tabela[make_pair(topo.first,(*it).id)]=="acc")
		{
			erro=0;
			break;
		}
		///////////////////////////////	
		////DEBUG: IMPRIMINDO PILHA////
		///////////////////////////////
		/*while(!pilha.empty())
		{
			teste.push_front(pilha.top());
			pilha.pop();
		}

		while(!teste.empty())
		{
			cout << teste.front().second << " " << teste.front().first << " ";
			pilha.push(teste.front());
			teste.pop_front();
		}
		cout << endl;
 		//////////////////////////
 		/////////////////////////
 		/////////////////////////
 		cout << "TOPO: " << topo.first << " " << topo.second << endl;
		*/if(tabela.find(make_pair(topo.first, (*it).id) ) == tabela.end())
		{
		//	cout << "vish\n";
		//	cout << (*it).data << endl;
			erro=1;
			break;
		}

		string s;

		s = tabela[make_pair(topo.first, (*it).id)]; // valor de shift, reduce ou transicao
		
		if(is_shift(s) && (*it).id!="$")
		{
			pilha.push(make_pair(to_int(s), *it));

			Node *tmp = newNodeSemPai(*it);

			semantic_stack.push(tmp);

			it++;
		}
		else if(is_reduce(s) || (*it).id=="$")
		{
			pair < int, string > par;
			par = transicoes[to_int(s)];

			int qtd_op = par.first;
			string cabeca_regra = par.second;

			aux_tk.id = aux_tk.data = cabeca_regra;
			
			no = newNodeSemPai(aux_tk);
		
			//pair < int, Token > semantic_topo;
			Node* semantic_topo;
			for(int i=0;i<qtd_op;i++)
			{
				semantic_topo = semantic_stack.top();
				no->filhos.insert(no->filhos.begin(), semantic_topo);
				semantic_stack.pop();				
				
				pilha.pop();

			}		

			int estado_pilha = pilha.top().first;

			if(tabela.find(make_pair(estado_pilha, cabeca_regra))==tabela.end())
			{
				erro=1;
			//	cout << "hey\n";
				break;
			}

			int new_estado_pilha = to_int(tabela[make_pair(estado_pilha,cabeca_regra)]);

			pilha.push(make_pair(new_estado_pilha, aux_tk));
			semantic_stack.push(no);
		}
	
	}

//	imprimeArvore(no);
	imprimeArvoreDFS(no);

	set<string>::iterator it_set;
	/*for(it_set=lista_funcs.begin();it_set!=lista_funcs.end();it_set++)
		cout << *it_set << endl;

	for(it_set=lista_vars.begin();it_set!=lista_vars.end();it_set++)
		cout << *it_set << endl;*/
/*	if(!erro)
		cout << "YES\n";
	else
		cout << "NO\n";
*/
	if(func_nao_declarada)
		cout << "YES\n";
	else
		cout << "NO\n";

	if(var_nao_declarada)
		cout << "YES\n";
	else
		cout << "NO\n";

	if(redecl_func)
		cout << "YES\n";
	else
		cout << "NO\n";

	if(redecl_var)
		cout << "YES\n";
	else
		cout << "NO\n";

	if(conflito_var_func)
		cout << "YES\n";
	else
		cout << "NO\n";


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

bool is_comparador(string lexema){
	if(lexema==">" || lexema=="<" || lexema=="==" || lexema=="!=" || lexema==">=" || lexema=="<=")
		return true;
	return false;
}

bool is_operador_arit(char c){
	if(c=='+' || c=='-' || c=='*' || c=='/' || c=='%')
		return true;
	return false;
}

bool is_shift(string s){
	if(s[0]=='s')
		return true;
	return false;
}

bool is_reduce(string s){
	if(s[0]=='r')
		return true;
	return false;
}

bool is_transition(string s){
	if(s[0]>='0' && s[0]<='9')
		return true;
	return false;
}

////////////////////////////////////////////////////////////////
////////////////						////////////////////////
////////////////	   AUXILIARES   	////////////////////////
////////////////						////////////////////////
////////////////////////////////////////////////////////////////

int to_int(string s){
	int result;
	if(is_shift(s) || is_reduce(s))
	{
		s.erase(0,1);
		result = stoi(s);
	}
	else{
		result = stoi(s);
	}
	return result;
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
				create_token(symbol_table, lexema, lexema); // PALAVRA RESERVADA
			else
				create_token(symbol_table, "id", lexema); // ID

			lexema.clear();
		}
	}
	else if(is_number(buffer[indice]))
	{
		estado=2;
		lexema+=buffer[indice];
		if(is_eof(buffer[indice+1]))
		{
			create_token(symbol_table, "number", lexema); // NUMBER
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
					create_token(symbol_table, "=", lexema); // ATRIBUICAO
				else
					create_token(symbol_table, lexema, lexema); // COMPARADOR
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
		create_token(symbol_table, lexema, lexema); // ; + - * / % ( ) [ ] { } ,
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
					create_token(symbol_table, lexema, lexema); // PALAVRA RESERVADA
				else
					create_token(symbol_table, "id", lexema); // ID

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
			create_token(symbol_table, lexema, lexema); // PALAVRA RESERVADA
		else
			create_token(symbol_table, "id", lexema); // ID

		lexema.clear();
	}
	else if(buffer[indice]=='\n')
	{
		estado=0;
		if(is_reserved_word(lexema, reserved_words))
			create_token(symbol_table, lexema, lexema); // PALAVRA RESERVADA
		else
			create_token(symbol_table, "id", lexema); // ID
		lexema.clear();
		linha++;
		coluna=0;
	}
	else
	{
		estado=0;
		if(is_reserved_word(lexema, reserved_words))
			create_token(symbol_table, lexema, lexema); // PALAVRA RESERVADA
		else
			create_token(symbol_table, "id", lexema); // ID
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
				create_token(symbol_table, "number", lexema); // NUMBER
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
		create_token(symbol_table, "number", lexema); // NUMBER 
		lexema.clear();
		achou_ponto=0;
	}
	else if(buffer[indice]=='\n')
	{
		estado=0;
		create_token(symbol_table, "number", lexema); // NUMBER
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
					create_token(symbol_table,"number",lexema); // NUMBER
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
		create_token(symbol_table,"number",lexema); // NUMBER
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
		create_token(symbol_table,lexema,lexema); // COMPARADOR
		lexema.clear();
		achou_exclamacao=0;
	}
	else if(buffer[indice]==' ' || buffer[indice]=='\t' || buffer[indice]=='\n')
	{
		if(!achou_exclamacao)
		{
			estado=0;
			create_token(symbol_table,lexema,lexema); // COMPARADOR
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
		create_token(symbol_table, lexema, lexema); // COMPARADOR
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
			create_token(symbol_table,"char",lexema); // CONST_CHAR
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
			create_token(symbol_table,"string",lexema); // CONST_STRING
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
			create_token(symbol_table,"string",lexema); // CONST_STRING
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

void preenche_tabela(map <pair<int,string>,string> &tabela){
tabela.insert(make_pair(make_pair(0, "int"), "s4"));
tabela.insert(make_pair(make_pair(0, "nada"), "s5"));
tabela.insert(make_pair(make_pair(0, "real"), "s6"));
tabela.insert(make_pair(make_pair(0, "letra"), "s7"));
tabela.insert(make_pair(make_pair(0, "PROTOTIPO_FUNC"), "1"));
tabela.insert(make_pair(make_pair(0, "FUNC"), "3"));
tabela.insert(make_pair(make_pair(0, "INT_MAINHA"), "8"));
tabela.insert(make_pair(make_pair(0, "TIPO_FUNC"), "2"));
tabela.insert(make_pair(make_pair(1, "$"), "acc"));
tabela.insert(make_pair(make_pair(2, "id"), "s9"));
tabela.insert(make_pair(make_pair(3, "$"), "r2"));
tabela.insert(make_pair(make_pair(4, "id"), "r67"));
tabela.insert(make_pair(make_pair(4, "mainha"), "s10"));
tabela.insert(make_pair(make_pair(5, "id"), "r68"));
tabela.insert(make_pair(make_pair(6, "id"), "r69"));
tabela.insert(make_pair(make_pair(7, "id"), "r70"));
tabela.insert(make_pair(make_pair(8, "$"), "r4"));
tabela.insert(make_pair(make_pair(9, "("), "s11"));
tabela.insert(make_pair(make_pair(10, "("), "s12"));
tabela.insert(make_pair(make_pair(11, "int"), "s16"));
tabela.insert(make_pair(make_pair(11, ")"), "s15"));
tabela.insert(make_pair(make_pair(11, "real"), "s17"));
tabela.insert(make_pair(make_pair(11, "letra"), "s18"));
tabela.insert(make_pair(make_pair(11, "TIPO_VAR"), "14"));
tabela.insert(make_pair(make_pair(11, "LISTA_VARS1"), "13"));
tabela.insert(make_pair(make_pair(12, ")"), "s19"));
tabela.insert(make_pair(make_pair(13, ";"), "s20"));
tabela.insert(make_pair(make_pair(13, "{"), "s21"));
tabela.insert(make_pair(make_pair(14, "id"), "s22"));
tabela.insert(make_pair(make_pair(15, ";"), "r77"));
tabela.insert(make_pair(make_pair(15, "{"), "r77"));
tabela.insert(make_pair(make_pair(16, "id"), "r71"));
tabela.insert(make_pair(make_pair(17, "id"), "r72"));
tabela.insert(make_pair(make_pair(18, "id"), "r73"));
tabela.insert(make_pair(make_pair(19, "{"), "s23"));
tabela.insert(make_pair(make_pair(20, "int"), "s4"));
tabela.insert(make_pair(make_pair(20, "nada"), "s5"));
tabela.insert(make_pair(make_pair(20, "real"), "s6"));
tabela.insert(make_pair(make_pair(20, "letra"), "s7"));
tabela.insert(make_pair(make_pair(20, "PROTOTIPO_FUNC"), "24"));
tabela.insert(make_pair(make_pair(20, "FUNC"), "3"));
tabela.insert(make_pair(make_pair(20, "INT_MAINHA"), "8"));
tabela.insert(make_pair(make_pair(20, "TIPO_FUNC"), "2"));
tabela.insert(make_pair(make_pair(21, "id"), "s38"));
tabela.insert(make_pair(make_pair(21, "int"), "s16"));
tabela.insert(make_pair(make_pair(21, "}"), "s37"));
tabela.insert(make_pair(make_pair(21, "se"), "s39"));
tabela.insert(make_pair(make_pair(21, "uai"), "s40"));
tabela.insert(make_pair(make_pair(21, "le"), "s41"));
tabela.insert(make_pair(make_pair(21, "escreve"), "s42"));
tabela.insert(make_pair(make_pair(21, "tome"), "s43"));
tabela.insert(make_pair(make_pair(21, "vaza"), "s44"));
tabela.insert(make_pair(make_pair(21, "real"), "s17"));
tabela.insert(make_pair(make_pair(21, "letra"), "s18"));
tabela.insert(make_pair(make_pair(21, "BLOCO_MAIN"), "25"));
tabela.insert(make_pair(make_pair(21, "DECLARACAO"), "26"));
tabela.insert(make_pair(make_pair(21, "BLOCO"), "28"));
tabela.insert(make_pair(make_pair(21, "ATRIBUICAO"), "29"));
tabela.insert(make_pair(make_pair(21, "DESVIO_CONDICIONAL"), "30"));
tabela.insert(make_pair(make_pair(21, "LOOP"), "31"));
tabela.insert(make_pair(make_pair(21, "LEITURA"), "32"));
tabela.insert(make_pair(make_pair(21, "ESCRITA"), "33"));
tabela.insert(make_pair(make_pair(21, "FUNC_NADA"), "34"));
tabela.insert(make_pair(make_pair(21, "TOME"), "35"));
tabela.insert(make_pair(make_pair(21, "VAZA"), "36"));
tabela.insert(make_pair(make_pair(21, "TIPO_VAR"), "27"));
tabela.insert(make_pair(make_pair(22, ")"), "s48"));
tabela.insert(make_pair(make_pair(22, "["), "s46"));
tabela.insert(make_pair(make_pair(22, ","), "s47"));
tabela.insert(make_pair(make_pair(22, "LISTA_VARS2"), "45"));
tabela.insert(make_pair(make_pair(23, "id"), "s38"));
tabela.insert(make_pair(make_pair(23, "int"), "s16"));
tabela.insert(make_pair(make_pair(23, "}"), "s37"));
tabela.insert(make_pair(make_pair(23, "se"), "s39"));
tabela.insert(make_pair(make_pair(23, "uai"), "s40"));
tabela.insert(make_pair(make_pair(23, "le"), "s41"));
tabela.insert(make_pair(make_pair(23, "escreve"), "s42"));
tabela.insert(make_pair(make_pair(23, "tome"), "s43"));
tabela.insert(make_pair(make_pair(23, "vaza"), "s44"));
tabela.insert(make_pair(make_pair(23, "real"), "s17"));
tabela.insert(make_pair(make_pair(23, "letra"), "s18"));
tabela.insert(make_pair(make_pair(23, "BLOCO_MAIN"), "49"));
tabela.insert(make_pair(make_pair(23, "DECLARACAO"), "26"));
tabela.insert(make_pair(make_pair(23, "BLOCO"), "28"));
tabela.insert(make_pair(make_pair(23, "ATRIBUICAO"), "29"));
tabela.insert(make_pair(make_pair(23, "DESVIO_CONDICIONAL"), "30"));
tabela.insert(make_pair(make_pair(23, "LOOP"), "31"));
tabela.insert(make_pair(make_pair(23, "LEITURA"), "32"));
tabela.insert(make_pair(make_pair(23, "ESCRITA"), "33"));
tabela.insert(make_pair(make_pair(23, "FUNC_NADA"), "34"));
tabela.insert(make_pair(make_pair(23, "TOME"), "35"));
tabela.insert(make_pair(make_pair(23, "VAZA"), "36"));
tabela.insert(make_pair(make_pair(23, "TIPO_VAR"), "27"));
tabela.insert(make_pair(make_pair(24, "$"), "r1"));
tabela.insert(make_pair(make_pair(25, "int"), "s4"));
tabela.insert(make_pair(make_pair(25, "nada"), "s5"));
tabela.insert(make_pair(make_pair(25, "real"), "s6"));
tabela.insert(make_pair(make_pair(25, "letra"), "s7"));
tabela.insert(make_pair(make_pair(25, "FUNC"), "50"));
tabela.insert(make_pair(make_pair(25, "INT_MAINHA"), "8"));
tabela.insert(make_pair(make_pair(25, "TIPO_FUNC"), "51"));
tabela.insert(make_pair(make_pair(26, "int"), "r6"));
tabela.insert(make_pair(make_pair(26, "nada"), "r6"));
tabela.insert(make_pair(make_pair(26, "real"), "r6"));
tabela.insert(make_pair(make_pair(26, "letra"), "r6"));
tabela.insert(make_pair(make_pair(26, "$"), "r6"));
tabela.insert(make_pair(make_pair(27, "id"), "s52"));
tabela.insert(make_pair(make_pair(28, "int"), "r10"));
tabela.insert(make_pair(make_pair(28, "nada"), "r10"));
tabela.insert(make_pair(make_pair(28, "real"), "r10"));
tabela.insert(make_pair(make_pair(28, "letra"), "r10"));
tabela.insert(make_pair(make_pair(28, "$"), "r10"));
tabela.insert(make_pair(make_pair(29, "id"), "s38"));
tabela.insert(make_pair(make_pair(29, "}"), "s37"));
tabela.insert(make_pair(make_pair(29, "se"), "s39"));
tabela.insert(make_pair(make_pair(29, "uai"), "s40"));
tabela.insert(make_pair(make_pair(29, "le"), "s41"));
tabela.insert(make_pair(make_pair(29, "escreve"), "s42"));
tabela.insert(make_pair(make_pair(29, "tome"), "s43"));
tabela.insert(make_pair(make_pair(29, "vaza"), "s44"));
tabela.insert(make_pair(make_pair(29, "BLOCO"), "53"));
tabela.insert(make_pair(make_pair(29, "ATRIBUICAO"), "29"));
tabela.insert(make_pair(make_pair(29, "DESVIO_CONDICIONAL"), "30"));
tabela.insert(make_pair(make_pair(29, "LOOP"), "31"));
tabela.insert(make_pair(make_pair(29, "LEITURA"), "32"));
tabela.insert(make_pair(make_pair(29, "ESCRITA"), "33"));
tabela.insert(make_pair(make_pair(29, "FUNC_NADA"), "34"));
tabela.insert(make_pair(make_pair(29, "TOME"), "35"));
tabela.insert(make_pair(make_pair(29, "VAZA"), "36"));
tabela.insert(make_pair(make_pair(30, "id"), "s38"));
tabela.insert(make_pair(make_pair(30, "}"), "s37"));
tabela.insert(make_pair(make_pair(30, "se"), "s39"));
tabela.insert(make_pair(make_pair(30, "uai"), "s40"));
tabela.insert(make_pair(make_pair(30, "le"), "s41"));
tabela.insert(make_pair(make_pair(30, "escreve"), "s42"));
tabela.insert(make_pair(make_pair(30, "tome"), "s43"));
tabela.insert(make_pair(make_pair(30, "vaza"), "s44"));
tabela.insert(make_pair(make_pair(30, "BLOCO"), "54"));
tabela.insert(make_pair(make_pair(30, "ATRIBUICAO"), "29"));
tabela.insert(make_pair(make_pair(30, "DESVIO_CONDICIONAL"), "30"));
tabela.insert(make_pair(make_pair(30, "LOOP"), "31"));
tabela.insert(make_pair(make_pair(30, "LEITURA"), "32"));
tabela.insert(make_pair(make_pair(30, "ESCRITA"), "33"));
tabela.insert(make_pair(make_pair(30, "FUNC_NADA"), "34"));
tabela.insert(make_pair(make_pair(30, "TOME"), "35"));
tabela.insert(make_pair(make_pair(30, "VAZA"), "36"));
tabela.insert(make_pair(make_pair(31, "id"), "s38"));
tabela.insert(make_pair(make_pair(31, "}"), "s37"));
tabela.insert(make_pair(make_pair(31, "se"), "s39"));
tabela.insert(make_pair(make_pair(31, "uai"), "s40"));
tabela.insert(make_pair(make_pair(31, "le"), "s41"));
tabela.insert(make_pair(make_pair(31, "escreve"), "s42"));
tabela.insert(make_pair(make_pair(31, "tome"), "s43"));
tabela.insert(make_pair(make_pair(31, "vaza"), "s44"));
tabela.insert(make_pair(make_pair(31, "BLOCO"), "55"));
tabela.insert(make_pair(make_pair(31, "ATRIBUICAO"), "29"));
tabela.insert(make_pair(make_pair(31, "DESVIO_CONDICIONAL"), "30"));
tabela.insert(make_pair(make_pair(31, "LOOP"), "31"));
tabela.insert(make_pair(make_pair(31, "LEITURA"), "32"));
tabela.insert(make_pair(make_pair(31, "ESCRITA"), "33"));
tabela.insert(make_pair(make_pair(31, "FUNC_NADA"), "34"));
tabela.insert(make_pair(make_pair(31, "TOME"), "35"));
tabela.insert(make_pair(make_pair(31, "VAZA"), "36"));
tabela.insert(make_pair(make_pair(32, "id"), "s38"));
tabela.insert(make_pair(make_pair(32, "}"), "s37"));
tabela.insert(make_pair(make_pair(32, "se"), "s39"));
tabela.insert(make_pair(make_pair(32, "uai"), "s40"));
tabela.insert(make_pair(make_pair(32, "le"), "s41"));
tabela.insert(make_pair(make_pair(32, "escreve"), "s42"));
tabela.insert(make_pair(make_pair(32, "tome"), "s43"));
tabela.insert(make_pair(make_pair(32, "vaza"), "s44"));
tabela.insert(make_pair(make_pair(32, "BLOCO"), "56"));
tabela.insert(make_pair(make_pair(32, "ATRIBUICAO"), "29"));
tabela.insert(make_pair(make_pair(32, "DESVIO_CONDICIONAL"), "30"));
tabela.insert(make_pair(make_pair(32, "LOOP"), "31"));
tabela.insert(make_pair(make_pair(32, "LEITURA"), "32"));
tabela.insert(make_pair(make_pair(32, "ESCRITA"), "33"));
tabela.insert(make_pair(make_pair(32, "FUNC_NADA"), "34"));
tabela.insert(make_pair(make_pair(32, "TOME"), "35"));
tabela.insert(make_pair(make_pair(32, "VAZA"), "36"));
tabela.insert(make_pair(make_pair(33, "id"), "s38"));
tabela.insert(make_pair(make_pair(33, "}"), "s37"));
tabela.insert(make_pair(make_pair(33, "se"), "s39"));
tabela.insert(make_pair(make_pair(33, "uai"), "s40"));
tabela.insert(make_pair(make_pair(33, "le"), "s41"));
tabela.insert(make_pair(make_pair(33, "escreve"), "s42"));
tabela.insert(make_pair(make_pair(33, "tome"), "s43"));
tabela.insert(make_pair(make_pair(33, "vaza"), "s44"));
tabela.insert(make_pair(make_pair(33, "BLOCO"), "57"));
tabela.insert(make_pair(make_pair(33, "ATRIBUICAO"), "29"));
tabela.insert(make_pair(make_pair(33, "DESVIO_CONDICIONAL"), "30"));
tabela.insert(make_pair(make_pair(33, "LOOP"), "31"));
tabela.insert(make_pair(make_pair(33, "LEITURA"), "32"));
tabela.insert(make_pair(make_pair(33, "ESCRITA"), "33"));
tabela.insert(make_pair(make_pair(33, "FUNC_NADA"), "34"));
tabela.insert(make_pair(make_pair(33, "TOME"), "35"));
tabela.insert(make_pair(make_pair(33, "VAZA"), "36"));
tabela.insert(make_pair(make_pair(34, "id"), "s38"));
tabela.insert(make_pair(make_pair(34, "}"), "s37"));
tabela.insert(make_pair(make_pair(34, "se"), "s39"));
tabela.insert(make_pair(make_pair(34, "uai"), "s40"));
tabela.insert(make_pair(make_pair(34, "le"), "s41"));
tabela.insert(make_pair(make_pair(34, "escreve"), "s42"));
tabela.insert(make_pair(make_pair(34, "tome"), "s43"));
tabela.insert(make_pair(make_pair(34, "vaza"), "s44"));
tabela.insert(make_pair(make_pair(34, "BLOCO"), "58"));
tabela.insert(make_pair(make_pair(34, "ATRIBUICAO"), "29"));
tabela.insert(make_pair(make_pair(34, "DESVIO_CONDICIONAL"), "30"));
tabela.insert(make_pair(make_pair(34, "LOOP"), "31"));
tabela.insert(make_pair(make_pair(34, "LEITURA"), "32"));
tabela.insert(make_pair(make_pair(34, "ESCRITA"), "33"));
tabela.insert(make_pair(make_pair(34, "FUNC_NADA"), "34"));
tabela.insert(make_pair(make_pair(34, "TOME"), "35"));
tabela.insert(make_pair(make_pair(34, "VAZA"), "36"));
tabela.insert(make_pair(make_pair(35, "id"), "s38"));
tabela.insert(make_pair(make_pair(35, "}"), "s37"));
tabela.insert(make_pair(make_pair(35, "se"), "s39"));
tabela.insert(make_pair(make_pair(35, "uai"), "s40"));
tabela.insert(make_pair(make_pair(35, "le"), "s41"));
tabela.insert(make_pair(make_pair(35, "escreve"), "s42"));
tabela.insert(make_pair(make_pair(35, "tome"), "s43"));
tabela.insert(make_pair(make_pair(35, "vaza"), "s44"));
tabela.insert(make_pair(make_pair(35, "BLOCO"), "59"));
tabela.insert(make_pair(make_pair(35, "ATRIBUICAO"), "29"));
tabela.insert(make_pair(make_pair(35, "DESVIO_CONDICIONAL"), "30"));
tabela.insert(make_pair(make_pair(35, "LOOP"), "31"));
tabela.insert(make_pair(make_pair(35, "LEITURA"), "32"));
tabela.insert(make_pair(make_pair(35, "ESCRITA"), "33"));
tabela.insert(make_pair(make_pair(35, "FUNC_NADA"), "34"));
tabela.insert(make_pair(make_pair(35, "TOME"), "35"));
tabela.insert(make_pair(make_pair(35, "VAZA"), "36"));
tabela.insert(make_pair(make_pair(36, "id"), "s38"));
tabela.insert(make_pair(make_pair(36, "}"), "s37"));
tabela.insert(make_pair(make_pair(36, "se"), "s39"));
tabela.insert(make_pair(make_pair(36, "uai"), "s40"));
tabela.insert(make_pair(make_pair(36, "le"), "s41"));
tabela.insert(make_pair(make_pair(36, "escreve"), "s42"));
tabela.insert(make_pair(make_pair(36, "tome"), "s43"));
tabela.insert(make_pair(make_pair(36, "vaza"), "s44"));
tabela.insert(make_pair(make_pair(36, "BLOCO"), "60"));
tabela.insert(make_pair(make_pair(36, "ATRIBUICAO"), "29"));
tabela.insert(make_pair(make_pair(36, "DESVIO_CONDICIONAL"), "30"));
tabela.insert(make_pair(make_pair(36, "LOOP"), "31"));
tabela.insert(make_pair(make_pair(36, "LEITURA"), "32"));
tabela.insert(make_pair(make_pair(36, "ESCRITA"), "33"));
tabela.insert(make_pair(make_pair(36, "FUNC_NADA"), "34"));
tabela.insert(make_pair(make_pair(36, "TOME"), "35"));
tabela.insert(make_pair(make_pair(36, "VAZA"), "36"));
tabela.insert(make_pair(make_pair(37, "id"), "r19"));
tabela.insert(make_pair(make_pair(37, "int"), "r19"));
tabela.insert(make_pair(make_pair(37, "}"), "r19"));
tabela.insert(make_pair(make_pair(37, "se"), "r19"));
tabela.insert(make_pair(make_pair(37, "senao"), "r19"));
tabela.insert(make_pair(make_pair(37, "uai"), "r19"));
tabela.insert(make_pair(make_pair(37, "le"), "r19"));
tabela.insert(make_pair(make_pair(37, "escreve"), "r19"));
tabela.insert(make_pair(make_pair(37, "tome"), "r19"));
tabela.insert(make_pair(make_pair(37, "vaza"), "r19"));
tabela.insert(make_pair(make_pair(37, "nada"), "r19"));
tabela.insert(make_pair(make_pair(37, "real"), "r19"));
tabela.insert(make_pair(make_pair(37, "letra"), "r19"));
tabela.insert(make_pair(make_pair(37, "$"), "r19"));
tabela.insert(make_pair(make_pair(38, "("), "s63"));
tabela.insert(make_pair(make_pair(38, "="), "s62"));
tabela.insert(make_pair(make_pair(38, "["), "s61"));
tabela.insert(make_pair(make_pair(39, "("), "s64"));
tabela.insert(make_pair(make_pair(40, "("), "s65"));
tabela.insert(make_pair(make_pair(41, "("), "s66"));
tabela.insert(make_pair(make_pair(42, "("), "s67"));
tabela.insert(make_pair(make_pair(43, "id"), "s75"));
tabela.insert(make_pair(make_pair(43, "("), "s74"));
tabela.insert(make_pair(make_pair(43, ";"), "s68"));
tabela.insert(make_pair(make_pair(43, "-"), "s72"));
tabela.insert(make_pair(make_pair(43, "number"), "s76"));
tabela.insert(make_pair(make_pair(43, "char"), "s77"));
tabela.insert(make_pair(make_pair(43, "EXP_ARITMETICA1"), "69"));
tabela.insert(make_pair(make_pair(43, "EXP_ARITMETICA2"), "70"));
tabela.insert(make_pair(make_pair(43, "EXP_ARITMETICA3"), "71"));
tabela.insert(make_pair(make_pair(43, "EXP_ARITMETICA4"), "73"));
tabela.insert(make_pair(make_pair(44, ";"), "s78"));
tabela.insert(make_pair(make_pair(45, ";"), "r74"));
tabela.insert(make_pair(make_pair(45, "{"), "r74"));
tabela.insert(make_pair(make_pair(46, "id"), "s75"));
tabela.insert(make_pair(make_pair(46, "("), "s74"));
tabela.insert(make_pair(make_pair(46, "]"), "s79"));
tabela.insert(make_pair(make_pair(46, "-"), "s72"));
tabela.insert(make_pair(make_pair(46, "number"), "s76"));
tabela.insert(make_pair(make_pair(46, "char"), "s77"));
tabela.insert(make_pair(make_pair(46, "EXP_ARITMETICA1"), "80"));
tabela.insert(make_pair(make_pair(46, "EXP_ARITMETICA2"), "70"));
tabela.insert(make_pair(make_pair(46, "EXP_ARITMETICA3"), "71"));
tabela.insert(make_pair(make_pair(46, "EXP_ARITMETICA4"), "73"));
tabela.insert(make_pair(make_pair(47, "int"), "s16"));
tabela.insert(make_pair(make_pair(47, "real"), "s17"));
tabela.insert(make_pair(make_pair(47, "letra"), "s18"));
tabela.insert(make_pair(make_pair(47, "TIPO_VAR"), "81"));
tabela.insert(make_pair(make_pair(48, ";"), "r81"));
tabela.insert(make_pair(make_pair(48, "{"), "r81"));
tabela.insert(make_pair(make_pair(49, "$"), "r5"));
tabela.insert(make_pair(make_pair(50, "$"), "r3"));
tabela.insert(make_pair(make_pair(51, "id"), "s82"));
tabela.insert(make_pair(make_pair(52, ";"), "s83"));
tabela.insert(make_pair(make_pair(52, "="), "s84"));
tabela.insert(make_pair(make_pair(52, "["), "s85"));
tabela.insert(make_pair(make_pair(53, "id"), "r11"));
tabela.insert(make_pair(make_pair(53, "int"), "r11"));
tabela.insert(make_pair(make_pair(53, "}"), "r11"));
tabela.insert(make_pair(make_pair(53, "se"), "r11"));
tabela.insert(make_pair(make_pair(53, "senao"), "r11"));
tabela.insert(make_pair(make_pair(53, "uai"), "r11"));
tabela.insert(make_pair(make_pair(53, "le"), "r11"));
tabela.insert(make_pair(make_pair(53, "escreve"), "r11"));
tabela.insert(make_pair(make_pair(53, "tome"), "r11"));
tabela.insert(make_pair(make_pair(53, "vaza"), "r11"));
tabela.insert(make_pair(make_pair(53, "nada"), "r11"));
tabela.insert(make_pair(make_pair(53, "real"), "r11"));
tabela.insert(make_pair(make_pair(53, "letra"), "r11"));
tabela.insert(make_pair(make_pair(53, "$"), "r11"));
tabela.insert(make_pair(make_pair(54, "id"), "r12"));
tabela.insert(make_pair(make_pair(54, "int"), "r12"));
tabela.insert(make_pair(make_pair(54, "}"), "r12"));
tabela.insert(make_pair(make_pair(54, "se"), "r12"));
tabela.insert(make_pair(make_pair(54, "senao"), "r12"));
tabela.insert(make_pair(make_pair(54, "uai"), "r12"));
tabela.insert(make_pair(make_pair(54, "le"), "r12"));
tabela.insert(make_pair(make_pair(54, "escreve"), "r12"));
tabela.insert(make_pair(make_pair(54, "tome"), "r12"));
tabela.insert(make_pair(make_pair(54, "vaza"), "r12"));
tabela.insert(make_pair(make_pair(54, "nada"), "r12"));
tabela.insert(make_pair(make_pair(54, "real"), "r12"));
tabela.insert(make_pair(make_pair(54, "letra"), "r12"));
tabela.insert(make_pair(make_pair(54, "$"), "r12"));
tabela.insert(make_pair(make_pair(55, "id"), "r13"));
tabela.insert(make_pair(make_pair(55, "int"), "r13"));
tabela.insert(make_pair(make_pair(55, "}"), "r13"));
tabela.insert(make_pair(make_pair(55, "se"), "r13"));
tabela.insert(make_pair(make_pair(55, "senao"), "r13"));
tabela.insert(make_pair(make_pair(55, "uai"), "r13"));
tabela.insert(make_pair(make_pair(55, "le"), "r13"));
tabela.insert(make_pair(make_pair(55, "escreve"), "r13"));
tabela.insert(make_pair(make_pair(55, "tome"), "r13"));
tabela.insert(make_pair(make_pair(55, "vaza"), "r13"));
tabela.insert(make_pair(make_pair(55, "nada"), "r13"));
tabela.insert(make_pair(make_pair(55, "real"), "r13"));
tabela.insert(make_pair(make_pair(55, "letra"), "r13"));
tabela.insert(make_pair(make_pair(55, "$"), "r13"));
tabela.insert(make_pair(make_pair(56, "id"), "r14"));
tabela.insert(make_pair(make_pair(56, "int"), "r14"));
tabela.insert(make_pair(make_pair(56, "}"), "r14"));
tabela.insert(make_pair(make_pair(56, "se"), "r14"));
tabela.insert(make_pair(make_pair(56, "senao"), "r14"));
tabela.insert(make_pair(make_pair(56, "uai"), "r14"));
tabela.insert(make_pair(make_pair(56, "le"), "r14"));
tabela.insert(make_pair(make_pair(56, "escreve"), "r14"));
tabela.insert(make_pair(make_pair(56, "tome"), "r14"));
tabela.insert(make_pair(make_pair(56, "vaza"), "r14"));
tabela.insert(make_pair(make_pair(56, "nada"), "r14"));
tabela.insert(make_pair(make_pair(56, "real"), "r14"));
tabela.insert(make_pair(make_pair(56, "letra"), "r14"));
tabela.insert(make_pair(make_pair(56, "$"), "r14"));
tabela.insert(make_pair(make_pair(57, "id"), "r15"));
tabela.insert(make_pair(make_pair(57, "int"), "r15"));
tabela.insert(make_pair(make_pair(57, "}"), "r15"));
tabela.insert(make_pair(make_pair(57, "se"), "r15"));
tabela.insert(make_pair(make_pair(57, "senao"), "r15"));
tabela.insert(make_pair(make_pair(57, "uai"), "r15"));
tabela.insert(make_pair(make_pair(57, "le"), "r15"));
tabela.insert(make_pair(make_pair(57, "escreve"), "r15"));
tabela.insert(make_pair(make_pair(57, "tome"), "r15"));
tabela.insert(make_pair(make_pair(57, "vaza"), "r15"));
tabela.insert(make_pair(make_pair(57, "nada"), "r15"));
tabela.insert(make_pair(make_pair(57, "real"), "r15"));
tabela.insert(make_pair(make_pair(57, "letra"), "r15"));
tabela.insert(make_pair(make_pair(57, "$"), "r15"));
tabela.insert(make_pair(make_pair(58, "id"), "r16"));
tabela.insert(make_pair(make_pair(58, "int"), "r16"));
tabela.insert(make_pair(make_pair(58, "}"), "r16"));
tabela.insert(make_pair(make_pair(58, "se"), "r16"));
tabela.insert(make_pair(make_pair(58, "senao"), "r16"));
tabela.insert(make_pair(make_pair(58, "uai"), "r16"));
tabela.insert(make_pair(make_pair(58, "le"), "r16"));
tabela.insert(make_pair(make_pair(58, "escreve"), "r16"));
tabela.insert(make_pair(make_pair(58, "tome"), "r16"));
tabela.insert(make_pair(make_pair(58, "vaza"), "r16"));
tabela.insert(make_pair(make_pair(58, "nada"), "r16"));
tabela.insert(make_pair(make_pair(58, "real"), "r16"));
tabela.insert(make_pair(make_pair(58, "letra"), "r16"));
tabela.insert(make_pair(make_pair(58, "$"), "r16"));
tabela.insert(make_pair(make_pair(59, "id"), "r17"));
tabela.insert(make_pair(make_pair(59, "int"), "r17"));
tabela.insert(make_pair(make_pair(59, "}"), "r17"));
tabela.insert(make_pair(make_pair(59, "se"), "r17"));
tabela.insert(make_pair(make_pair(59, "senao"), "r17"));
tabela.insert(make_pair(make_pair(59, "uai"), "r17"));
tabela.insert(make_pair(make_pair(59, "le"), "r17"));
tabela.insert(make_pair(make_pair(59, "escreve"), "r17"));
tabela.insert(make_pair(make_pair(59, "tome"), "r17"));
tabela.insert(make_pair(make_pair(59, "vaza"), "r17"));
tabela.insert(make_pair(make_pair(59, "nada"), "r17"));
tabela.insert(make_pair(make_pair(59, "real"), "r17"));
tabela.insert(make_pair(make_pair(59, "letra"), "r17"));
tabela.insert(make_pair(make_pair(59, "$"), "r17"));
tabela.insert(make_pair(make_pair(60, "id"), "r18"));
tabela.insert(make_pair(make_pair(60, "int"), "r18"));
tabela.insert(make_pair(make_pair(60, "}"), "r18"));
tabela.insert(make_pair(make_pair(60, "se"), "r18"));
tabela.insert(make_pair(make_pair(60, "senao"), "r18"));
tabela.insert(make_pair(make_pair(60, "uai"), "r18"));
tabela.insert(make_pair(make_pair(60, "le"), "r18"));
tabela.insert(make_pair(make_pair(60, "escreve"), "r18"));
tabela.insert(make_pair(make_pair(60, "tome"), "r18"));
tabela.insert(make_pair(make_pair(60, "vaza"), "r18"));
tabela.insert(make_pair(make_pair(60, "nada"), "r18"));
tabela.insert(make_pair(make_pair(60, "real"), "r18"));
tabela.insert(make_pair(make_pair(60, "letra"), "r18"));
tabela.insert(make_pair(make_pair(60, "$"), "r18"));
tabela.insert(make_pair(make_pair(61, "id"), "s75"));
tabela.insert(make_pair(make_pair(61, "("), "s74"));
tabela.insert(make_pair(make_pair(61, "-"), "s72"));
tabela.insert(make_pair(make_pair(61, "number"), "s76"));
tabela.insert(make_pair(make_pair(61, "char"), "s77"));
tabela.insert(make_pair(make_pair(61, "EXP_ARITMETICA1"), "86"));
tabela.insert(make_pair(make_pair(61, "EXP_ARITMETICA2"), "70"));
tabela.insert(make_pair(make_pair(61, "EXP_ARITMETICA3"), "71"));
tabela.insert(make_pair(make_pair(61, "EXP_ARITMETICA4"), "73"));
tabela.insert(make_pair(make_pair(62, "id"), "s75"));
tabela.insert(make_pair(make_pair(62, "("), "s74"));
tabela.insert(make_pair(make_pair(62, "-"), "s72"));
tabela.insert(make_pair(make_pair(62, "number"), "s76"));
tabela.insert(make_pair(make_pair(62, "char"), "s77"));
tabela.insert(make_pair(make_pair(62, "EXP_ARITMETICA1"), "87"));
tabela.insert(make_pair(make_pair(62, "EXP_ARITMETICA2"), "70"));
tabela.insert(make_pair(make_pair(62, "EXP_ARITMETICA3"), "71"));
tabela.insert(make_pair(make_pair(62, "EXP_ARITMETICA4"), "73"));
tabela.insert(make_pair(make_pair(63, "id"), "s75"));
tabela.insert(make_pair(make_pair(63, "("), "s74"));
tabela.insert(make_pair(make_pair(63, ")"), "s90"));
tabela.insert(make_pair(make_pair(63, "-"), "s72"));
tabela.insert(make_pair(make_pair(63, "number"), "s76"));
tabela.insert(make_pair(make_pair(63, "char"), "s77"));
tabela.insert(make_pair(make_pair(63, "EXP_ARITMETICA1"), "89"));
tabela.insert(make_pair(make_pair(63, "EXP_ARITMETICA2"), "70"));
tabela.insert(make_pair(make_pair(63, "EXP_ARITMETICA3"), "71"));
tabela.insert(make_pair(make_pair(63, "EXP_ARITMETICA4"), "73"));
tabela.insert(make_pair(make_pair(63, "FUNC_ARG1"), "88"));
tabela.insert(make_pair(make_pair(64, "id"), "s75"));
tabela.insert(make_pair(make_pair(64, "("), "s96"));
tabela.insert(make_pair(make_pair(64, "-"), "s72"));
tabela.insert(make_pair(make_pair(64, "number"), "s76"));
tabela.insert(make_pair(make_pair(64, "char"), "s77"));
tabela.insert(make_pair(make_pair(64, "nao"), "s94"));
tabela.insert(make_pair(make_pair(64, "EXP_ARITMETICA1"), "98"));
tabela.insert(make_pair(make_pair(64, "EXP_ARITMETICA2"), "70"));
tabela.insert(make_pair(make_pair(64, "EXP_ARITMETICA3"), "71"));
tabela.insert(make_pair(make_pair(64, "EXP_ARITMETICA4"), "73"));
tabela.insert(make_pair(make_pair(64, "EXP_RELACIONAL"), "97"));
tabela.insert(make_pair(make_pair(64, "EXP_COMP1"), "91"));
tabela.insert(make_pair(make_pair(64, "EXP_COMP2"), "92"));
tabela.insert(make_pair(make_pair(64, "EXP_COMP3"), "93"));
tabela.insert(make_pair(make_pair(64, "EXP_COMP4"), "95"));
tabela.insert(make_pair(make_pair(65, "id"), "s75"));
tabela.insert(make_pair(make_pair(65, "("), "s96"));
tabela.insert(make_pair(make_pair(65, "-"), "s72"));
tabela.insert(make_pair(make_pair(65, "number"), "s76"));
tabela.insert(make_pair(make_pair(65, "char"), "s77"));
tabela.insert(make_pair(make_pair(65, "nao"), "s94"));
tabela.insert(make_pair(make_pair(65, "EXP_ARITMETICA1"), "98"));
tabela.insert(make_pair(make_pair(65, "EXP_ARITMETICA2"), "70"));
tabela.insert(make_pair(make_pair(65, "EXP_ARITMETICA3"), "71"));
tabela.insert(make_pair(make_pair(65, "EXP_ARITMETICA4"), "73"));
tabela.insert(make_pair(make_pair(65, "EXP_RELACIONAL"), "97"));
tabela.insert(make_pair(make_pair(65, "EXP_COMP1"), "99"));
tabela.insert(make_pair(make_pair(65, "EXP_COMP2"), "92"));
tabela.insert(make_pair(make_pair(65, "EXP_COMP3"), "93"));
tabela.insert(make_pair(make_pair(65, "EXP_COMP4"), "95"));
tabela.insert(make_pair(make_pair(66, "id"), "s100"));
tabela.insert(make_pair(make_pair(67, "id"), "s75"));
tabela.insert(make_pair(make_pair(67, "("), "s74"));
tabela.insert(make_pair(make_pair(67, "string"), "s102"));
tabela.insert(make_pair(make_pair(67, "-"), "s72"));
tabela.insert(make_pair(make_pair(67, "number"), "s76"));
tabela.insert(make_pair(make_pair(67, "char"), "s77"));
tabela.insert(make_pair(make_pair(67, "EXP_ARITMETICA1"), "101"));
tabela.insert(make_pair(make_pair(67, "EXP_ARITMETICA2"), "70"));
tabela.insert(make_pair(make_pair(67, "EXP_ARITMETICA3"), "71"));
tabela.insert(make_pair(make_pair(67, "EXP_ARITMETICA4"), "73"));
tabela.insert(make_pair(make_pair(68, "id"), "r31"));
tabela.insert(make_pair(make_pair(68, "}"), "r31"));
tabela.insert(make_pair(make_pair(68, "se"), "r31"));
tabela.insert(make_pair(make_pair(68, "uai"), "r31"));
tabela.insert(make_pair(make_pair(68, "le"), "r31"));
tabela.insert(make_pair(make_pair(68, "escreve"), "r31"));
tabela.insert(make_pair(make_pair(68, "tome"), "r31"));
tabela.insert(make_pair(make_pair(68, "vaza"), "r31"));
tabela.insert(make_pair(make_pair(69, ";"), "s103"));
tabela.insert(make_pair(make_pair(69, "+"), "s104"));
tabela.insert(make_pair(make_pair(69, "-"), "s105"));
tabela.insert(make_pair(make_pair(70, ";"), "r36"));
tabela.insert(make_pair(make_pair(70, ")"), "r36"));
tabela.insert(make_pair(make_pair(70, "]"), "r36"));
tabela.insert(make_pair(make_pair(70, "+"), "r36"));
tabela.insert(make_pair(make_pair(70, "-"), "r36"));
tabela.insert(make_pair(make_pair(70, "*"), "s106"));
tabela.insert(make_pair(make_pair(70, "/"), "s107"));
tabela.insert(make_pair(make_pair(70, "%"), "s108"));
tabela.insert(make_pair(make_pair(70, ","), "r36"));
tabela.insert(make_pair(make_pair(70, "=="), "r36"));
tabela.insert(make_pair(make_pair(70, "!="), "r36"));
tabela.insert(make_pair(make_pair(70, ">="), "r36"));
tabela.insert(make_pair(make_pair(70, "<="), "r36"));
tabela.insert(make_pair(make_pair(70, ">"), "r36"));
tabela.insert(make_pair(make_pair(70, "<"), "r36"));
tabela.insert(make_pair(make_pair(70, "ou"), "r36"));
tabela.insert(make_pair(make_pair(70, "e"), "r36"));
tabela.insert(make_pair(make_pair(71, ";"), "r40"));
tabela.insert(make_pair(make_pair(71, ")"), "r40"));
tabela.insert(make_pair(make_pair(71, "]"), "r40"));
tabela.insert(make_pair(make_pair(71, "+"), "r40"));
tabela.insert(make_pair(make_pair(71, "-"), "r40"));
tabela.insert(make_pair(make_pair(71, "*"), "r40"));
tabela.insert(make_pair(make_pair(71, "/"), "r40"));
tabela.insert(make_pair(make_pair(71, "%"), "r40"));
tabela.insert(make_pair(make_pair(71, ","), "r40"));
tabela.insert(make_pair(make_pair(71, "=="), "r40"));
tabela.insert(make_pair(make_pair(71, "!="), "r40"));
tabela.insert(make_pair(make_pair(71, ">="), "r40"));
tabela.insert(make_pair(make_pair(71, "<="), "r40"));
tabela.insert(make_pair(make_pair(71, ">"), "r40"));
tabela.insert(make_pair(make_pair(71, "<"), "r40"));
tabela.insert(make_pair(make_pair(71, "ou"), "r40"));
tabela.insert(make_pair(make_pair(71, "e"), "r40"));
tabela.insert(make_pair(make_pair(72, "id"), "s75"));
tabela.insert(make_pair(make_pair(72, "("), "s74"));
tabela.insert(make_pair(make_pair(72, "number"), "s76"));
tabela.insert(make_pair(make_pair(72, "char"), "s77"));
tabela.insert(make_pair(make_pair(72, "EXP_ARITMETICA4"), "109"));
tabela.insert(make_pair(make_pair(73, ";"), "r42"));
tabela.insert(make_pair(make_pair(73, ")"), "r42"));
tabela.insert(make_pair(make_pair(73, "]"), "r42"));
tabela.insert(make_pair(make_pair(73, "+"), "r42"));
tabela.insert(make_pair(make_pair(73, "-"), "r42"));
tabela.insert(make_pair(make_pair(73, "*"), "r42"));
tabela.insert(make_pair(make_pair(73, "/"), "r42"));
tabela.insert(make_pair(make_pair(73, "%"), "r42"));
tabela.insert(make_pair(make_pair(73, ","), "r42"));
tabela.insert(make_pair(make_pair(73, "=="), "r42"));
tabela.insert(make_pair(make_pair(73, "!="), "r42"));
tabela.insert(make_pair(make_pair(73, ">="), "r42"));
tabela.insert(make_pair(make_pair(73, "<="), "r42"));
tabela.insert(make_pair(make_pair(73, ">"), "r42"));
tabela.insert(make_pair(make_pair(73, "<"), "r42"));
tabela.insert(make_pair(make_pair(73, "ou"), "r42"));
tabela.insert(make_pair(make_pair(73, "e"), "r42"));
tabela.insert(make_pair(make_pair(74, "id"), "s75"));
tabela.insert(make_pair(make_pair(74, "("), "s74"));
tabela.insert(make_pair(make_pair(74, "-"), "s72"));
tabela.insert(make_pair(make_pair(74, "number"), "s76"));
tabela.insert(make_pair(make_pair(74, "char"), "s77"));
tabela.insert(make_pair(make_pair(74, "EXP_ARITMETICA1"), "110"));
tabela.insert(make_pair(make_pair(74, "EXP_ARITMETICA2"), "70"));
tabela.insert(make_pair(make_pair(74, "EXP_ARITMETICA3"), "71"));
tabela.insert(make_pair(make_pair(74, "EXP_ARITMETICA4"), "73"));
tabela.insert(make_pair(make_pair(75, "("), "s112"));
tabela.insert(make_pair(make_pair(75, ";"), "r44"));
tabela.insert(make_pair(make_pair(75, ")"), "r44"));
tabela.insert(make_pair(make_pair(75, "["), "s111"));
tabela.insert(make_pair(make_pair(75, "]"), "r44"));
tabela.insert(make_pair(make_pair(75, "+"), "r44"));
tabela.insert(make_pair(make_pair(75, "-"), "r44"));
tabela.insert(make_pair(make_pair(75, "*"), "r44"));
tabela.insert(make_pair(make_pair(75, "/"), "r44"));
tabela.insert(make_pair(make_pair(75, "%"), "r44"));
tabela.insert(make_pair(make_pair(75, ","), "r44"));
tabela.insert(make_pair(make_pair(75, "=="), "r44"));
tabela.insert(make_pair(make_pair(75, "!="), "r44"));
tabela.insert(make_pair(make_pair(75, ">="), "r44"));
tabela.insert(make_pair(make_pair(75, "<="), "r44"));
tabela.insert(make_pair(make_pair(75, ">"), "r44"));
tabela.insert(make_pair(make_pair(75, "<"), "r44"));
tabela.insert(make_pair(make_pair(75, "ou"), "r44"));
tabela.insert(make_pair(make_pair(75, "e"), "r44"));
tabela.insert(make_pair(make_pair(76, ";"), "r45"));
tabela.insert(make_pair(make_pair(76, ")"), "r45"));
tabela.insert(make_pair(make_pair(76, "]"), "r45"));
tabela.insert(make_pair(make_pair(76, "+"), "r45"));
tabela.insert(make_pair(make_pair(76, "-"), "r45"));
tabela.insert(make_pair(make_pair(76, "*"), "r45"));
tabela.insert(make_pair(make_pair(76, "/"), "r45"));
tabela.insert(make_pair(make_pair(76, "%"), "r45"));
tabela.insert(make_pair(make_pair(76, ","), "r45"));
tabela.insert(make_pair(make_pair(76, "=="), "r45"));
tabela.insert(make_pair(make_pair(76, "!="), "r45"));
tabela.insert(make_pair(make_pair(76, ">="), "r45"));
tabela.insert(make_pair(make_pair(76, "<="), "r45"));
tabela.insert(make_pair(make_pair(76, ">"), "r45"));
tabela.insert(make_pair(make_pair(76, "<"), "r45"));
tabela.insert(make_pair(make_pair(76, "ou"), "r45"));
tabela.insert(make_pair(make_pair(76, "e"), "r45"));
tabela.insert(make_pair(make_pair(77, ";"), "r46"));
tabela.insert(make_pair(make_pair(77, ")"), "r46"));
tabela.insert(make_pair(make_pair(77, "]"), "r46"));
tabela.insert(make_pair(make_pair(77, "+"), "r46"));
tabela.insert(make_pair(make_pair(77, "-"), "r46"));
tabela.insert(make_pair(make_pair(77, "*"), "r46"));
tabela.insert(make_pair(make_pair(77, "/"), "r46"));
tabela.insert(make_pair(make_pair(77, "%"), "r46"));
tabela.insert(make_pair(make_pair(77, ","), "r46"));
tabela.insert(make_pair(make_pair(77, "=="), "r46"));
tabela.insert(make_pair(make_pair(77, "!="), "r46"));
tabela.insert(make_pair(make_pair(77, ">="), "r46"));
tabela.insert(make_pair(make_pair(77, "<="), "r46"));
tabela.insert(make_pair(make_pair(77, ">"), "r46"));
tabela.insert(make_pair(make_pair(77, "<"), "r46"));
tabela.insert(make_pair(make_pair(77, "ou"), "r46"));
tabela.insert(make_pair(make_pair(77, "e"), "r46"));
tabela.insert(make_pair(make_pair(78, "id"), "r33"));
tabela.insert(make_pair(make_pair(78, "}"), "r33"));
tabela.insert(make_pair(make_pair(78, "se"), "r33"));
tabela.insert(make_pair(make_pair(78, "uai"), "r33"));
tabela.insert(make_pair(make_pair(78, "le"), "r33"));
tabela.insert(make_pair(make_pair(78, "escreve"), "r33"));
tabela.insert(make_pair(make_pair(78, "tome"), "r33"));
tabela.insert(make_pair(make_pair(78, "vaza"), "r33"));
tabela.insert(make_pair(make_pair(79, ")"), "s48"));
tabela.insert(make_pair(make_pair(79, ","), "s47"));
tabela.insert(make_pair(make_pair(79, "LISTA_VARS2"), "113"));
tabela.insert(make_pair(make_pair(80, "]"), "s114"));
tabela.insert(make_pair(make_pair(80, "+"), "s104"));
tabela.insert(make_pair(make_pair(80, "-"), "s105"));
tabela.insert(make_pair(make_pair(81, "id"), "s115"));
tabela.insert(make_pair(make_pair(82, "("), "s116"));
tabela.insert(make_pair(make_pair(83, "id"), "s38"));
tabela.insert(make_pair(make_pair(83, "int"), "s16"));
tabela.insert(make_pair(make_pair(83, "}"), "s37"));
tabela.insert(make_pair(make_pair(83, "se"), "s39"));
tabela.insert(make_pair(make_pair(83, "uai"), "s40"));
tabela.insert(make_pair(make_pair(83, "le"), "s41"));
tabela.insert(make_pair(make_pair(83, "escreve"), "s42"));
tabela.insert(make_pair(make_pair(83, "tome"), "s43"));
tabela.insert(make_pair(make_pair(83, "vaza"), "s44"));
tabela.insert(make_pair(make_pair(83, "real"), "s17"));
tabela.insert(make_pair(make_pair(83, "letra"), "s18"));
tabela.insert(make_pair(make_pair(83, "DECLARACAO"), "117"));
tabela.insert(make_pair(make_pair(83, "BLOCO"), "28"));
tabela.insert(make_pair(make_pair(83, "ATRIBUICAO"), "29"));
tabela.insert(make_pair(make_pair(83, "DESVIO_CONDICIONAL"), "30"));
tabela.insert(make_pair(make_pair(83, "LOOP"), "31"));
tabela.insert(make_pair(make_pair(83, "LEITURA"), "32"));
tabela.insert(make_pair(make_pair(83, "ESCRITA"), "33"));
tabela.insert(make_pair(make_pair(83, "FUNC_NADA"), "34"));
tabela.insert(make_pair(make_pair(83, "TOME"), "35"));
tabela.insert(make_pair(make_pair(83, "VAZA"), "36"));
tabela.insert(make_pair(make_pair(83, "TIPO_VAR"), "27"));
tabela.insert(make_pair(make_pair(84, "id"), "s75"));
tabela.insert(make_pair(make_pair(84, "("), "s74"));
tabela.insert(make_pair(make_pair(84, "-"), "s72"));
tabela.insert(make_pair(make_pair(84, "number"), "s76"));
tabela.insert(make_pair(make_pair(84, "char"), "s77"));
tabela.insert(make_pair(make_pair(84, "EXP_ARITMETICA1"), "118"));
tabela.insert(make_pair(make_pair(84, "EXP_ARITMETICA2"), "70"));
tabela.insert(make_pair(make_pair(84, "EXP_ARITMETICA3"), "71"));
tabela.insert(make_pair(make_pair(84, "EXP_ARITMETICA4"), "73"));
tabela.insert(make_pair(make_pair(85, "id"), "s75"));
tabela.insert(make_pair(make_pair(85, "("), "s74"));
tabela.insert(make_pair(make_pair(85, "-"), "s72"));
tabela.insert(make_pair(make_pair(85, "number"), "s76"));
tabela.insert(make_pair(make_pair(85, "char"), "s77"));
tabela.insert(make_pair(make_pair(85, "EXP_ARITMETICA1"), "119"));
tabela.insert(make_pair(make_pair(85, "EXP_ARITMETICA2"), "70"));
tabela.insert(make_pair(make_pair(85, "EXP_ARITMETICA3"), "71"));
tabela.insert(make_pair(make_pair(85, "EXP_ARITMETICA4"), "73"));
tabela.insert(make_pair(make_pair(86, "]"), "s120"));
tabela.insert(make_pair(make_pair(86, "+"), "s104"));
tabela.insert(make_pair(make_pair(86, "-"), "s105"));
tabela.insert(make_pair(make_pair(87, ";"), "s121"));
tabela.insert(make_pair(make_pair(87, "+"), "s104"));
tabela.insert(make_pair(make_pair(87, "-"), "s105"));
tabela.insert(make_pair(make_pair(88, ";"), "s122"));
tabela.insert(make_pair(make_pair(89, ")"), "s125"));
tabela.insert(make_pair(make_pair(89, "+"), "s104"));
tabela.insert(make_pair(make_pair(89, "-"), "s105"));
tabela.insert(make_pair(make_pair(89, ","), "s124"));
tabela.insert(make_pair(make_pair(89, "FUNC_ARG2"), "123"));
tabela.insert(make_pair(make_pair(90, ";"), "r50"));
tabela.insert(make_pair(make_pair(90, ")"), "r50"));
tabela.insert(make_pair(make_pair(90, "]"), "r50"));
tabela.insert(make_pair(make_pair(90, "+"), "r50"));
tabela.insert(make_pair(make_pair(90, "-"), "r50"));
tabela.insert(make_pair(make_pair(90, "*"), "r50"));
tabela.insert(make_pair(make_pair(90, "/"), "r50"));
tabela.insert(make_pair(make_pair(90, "%"), "r50"));
tabela.insert(make_pair(make_pair(90, ","), "r50"));
tabela.insert(make_pair(make_pair(90, "=="), "r50"));
tabela.insert(make_pair(make_pair(90, "!="), "r50"));
tabela.insert(make_pair(make_pair(90, ">="), "r50"));
tabela.insert(make_pair(make_pair(90, "<="), "r50"));
tabela.insert(make_pair(make_pair(90, ">"), "r50"));
tabela.insert(make_pair(make_pair(90, "<"), "r50"));
tabela.insert(make_pair(make_pair(90, "ou"), "r50"));
tabela.insert(make_pair(make_pair(90, "e"), "r50"));
tabela.insert(make_pair(make_pair(91, ")"), "s126"));
tabela.insert(make_pair(make_pair(91, "ou"), "s127"));
tabela.insert(make_pair(make_pair(92, ")"), "r60"));
tabela.insert(make_pair(make_pair(92, "ou"), "r60"));
tabela.insert(make_pair(make_pair(92, "e"), "s128"));
tabela.insert(make_pair(make_pair(93, ")"), "r62"));
tabela.insert(make_pair(make_pair(93, "ou"), "r62"));
tabela.insert(make_pair(make_pair(93, "e"), "r62"));
tabela.insert(make_pair(make_pair(94, "id"), "s75"));
tabela.insert(make_pair(make_pair(94, "("), "s96"));
tabela.insert(make_pair(make_pair(94, "-"), "s72"));
tabela.insert(make_pair(make_pair(94, "number"), "s76"));
tabela.insert(make_pair(make_pair(94, "char"), "s77"));
tabela.insert(make_pair(make_pair(94, "nao"), "s94"));
tabela.insert(make_pair(make_pair(94, "EXP_ARITMETICA1"), "98"));
tabela.insert(make_pair(make_pair(94, "EXP_ARITMETICA2"), "70"));
tabela.insert(make_pair(make_pair(94, "EXP_ARITMETICA3"), "71"));
tabela.insert(make_pair(make_pair(94, "EXP_ARITMETICA4"), "73"));
tabela.insert(make_pair(make_pair(94, "EXP_RELACIONAL"), "97"));
tabela.insert(make_pair(make_pair(94, "EXP_COMP3"), "129"));
tabela.insert(make_pair(make_pair(94, "EXP_COMP4"), "95"));
tabela.insert(make_pair(make_pair(95, ")"), "r64"));
tabela.insert(make_pair(make_pair(95, "ou"), "r64"));
tabela.insert(make_pair(make_pair(95, "e"), "r64"));
tabela.insert(make_pair(make_pair(96, "id"), "s75"));
tabela.insert(make_pair(make_pair(96, "("), "s96"));
tabela.insert(make_pair(make_pair(96, "-"), "s72"));
tabela.insert(make_pair(make_pair(96, "number"), "s76"));
tabela.insert(make_pair(make_pair(96, "char"), "s77"));
tabela.insert(make_pair(make_pair(96, "nao"), "s94"));
tabela.insert(make_pair(make_pair(96, "EXP_ARITMETICA1"), "131"));
tabela.insert(make_pair(make_pair(96, "EXP_ARITMETICA2"), "70"));
tabela.insert(make_pair(make_pair(96, "EXP_ARITMETICA3"), "71"));
tabela.insert(make_pair(make_pair(96, "EXP_ARITMETICA4"), "73"));
tabela.insert(make_pair(make_pair(96, "EXP_RELACIONAL"), "97"));
tabela.insert(make_pair(make_pair(96, "EXP_COMP1"), "130"));
tabela.insert(make_pair(make_pair(96, "EXP_COMP2"), "92"));
tabela.insert(make_pair(make_pair(96, "EXP_COMP3"), "93"));
tabela.insert(make_pair(make_pair(96, "EXP_COMP4"), "95"));
tabela.insert(make_pair(make_pair(97, ")"), "r66"));
tabela.insert(make_pair(make_pair(97, "ou"), "r66"));
tabela.insert(make_pair(make_pair(97, "e"), "r66"));
tabela.insert(make_pair(make_pair(98, "+"), "s104"));
tabela.insert(make_pair(make_pair(98, "-"), "s105"));
tabela.insert(make_pair(make_pair(98, "=="), "s132"));
tabela.insert(make_pair(make_pair(98, "!="), "s133"));
tabela.insert(make_pair(make_pair(98, ">="), "s134"));
tabela.insert(make_pair(make_pair(98, "<="), "s135"));
tabela.insert(make_pair(make_pair(98, ">"), "s136"));
tabela.insert(make_pair(make_pair(98, "<"), "s137"));
tabela.insert(make_pair(make_pair(99, ")"), "s138"));
tabela.insert(make_pair(make_pair(99, "ou"), "s127"));
tabela.insert(make_pair(make_pair(100, ")"), "s139"));
tabela.insert(make_pair(make_pair(100, "["), "s140"));
tabela.insert(make_pair(make_pair(101, ")"), "s141"));
tabela.insert(make_pair(make_pair(101, "+"), "s104"));
tabela.insert(make_pair(make_pair(101, "-"), "s105"));
tabela.insert(make_pair(make_pair(102, ")"), "s142"));
tabela.insert(make_pair(make_pair(103, "id"), "r32"));
tabela.insert(make_pair(make_pair(103, "}"), "r32"));
tabela.insert(make_pair(make_pair(103, "se"), "r32"));
tabela.insert(make_pair(make_pair(103, "uai"), "r32"));
tabela.insert(make_pair(make_pair(103, "le"), "r32"));
tabela.insert(make_pair(make_pair(103, "escreve"), "r32"));
tabela.insert(make_pair(make_pair(103, "tome"), "r32"));
tabela.insert(make_pair(make_pair(103, "vaza"), "r32"));
tabela.insert(make_pair(make_pair(104, "id"), "s75"));
tabela.insert(make_pair(make_pair(104, "("), "s74"));
tabela.insert(make_pair(make_pair(104, "-"), "s72"));
tabela.insert(make_pair(make_pair(104, "number"), "s76"));
tabela.insert(make_pair(make_pair(104, "char"), "s77"));
tabela.insert(make_pair(make_pair(104, "EXP_ARITMETICA2"), "143"));
tabela.insert(make_pair(make_pair(104, "EXP_ARITMETICA3"), "71"));
tabela.insert(make_pair(make_pair(104, "EXP_ARITMETICA4"), "73"));
tabela.insert(make_pair(make_pair(105, "id"), "s75"));
tabela.insert(make_pair(make_pair(105, "("), "s74"));
tabela.insert(make_pair(make_pair(105, "-"), "s72"));
tabela.insert(make_pair(make_pair(105, "number"), "s76"));
tabela.insert(make_pair(make_pair(105, "char"), "s77"));
tabela.insert(make_pair(make_pair(105, "EXP_ARITMETICA2"), "144"));
tabela.insert(make_pair(make_pair(105, "EXP_ARITMETICA3"), "71"));
tabela.insert(make_pair(make_pair(105, "EXP_ARITMETICA4"), "73"));
tabela.insert(make_pair(make_pair(106, "id"), "s75"));
tabela.insert(make_pair(make_pair(106, "("), "s74"));
tabela.insert(make_pair(make_pair(106, "-"), "s72"));
tabela.insert(make_pair(make_pair(106, "number"), "s76"));
tabela.insert(make_pair(make_pair(106, "char"), "s77"));
tabela.insert(make_pair(make_pair(106, "EXP_ARITMETICA3"), "145"));
tabela.insert(make_pair(make_pair(106, "EXP_ARITMETICA4"), "73"));
tabela.insert(make_pair(make_pair(107, "id"), "s75"));
tabela.insert(make_pair(make_pair(107, "("), "s74"));
tabela.insert(make_pair(make_pair(107, "-"), "s72"));
tabela.insert(make_pair(make_pair(107, "number"), "s76"));
tabela.insert(make_pair(make_pair(107, "char"), "s77"));
tabela.insert(make_pair(make_pair(107, "EXP_ARITMETICA3"), "146"));
tabela.insert(make_pair(make_pair(107, "EXP_ARITMETICA4"), "73"));
tabela.insert(make_pair(make_pair(108, "id"), "s75"));
tabela.insert(make_pair(make_pair(108, "("), "s74"));
tabela.insert(make_pair(make_pair(108, "-"), "s72"));
tabela.insert(make_pair(make_pair(108, "number"), "s76"));
tabela.insert(make_pair(make_pair(108, "char"), "s77"));
tabela.insert(make_pair(make_pair(108, "EXP_ARITMETICA3"), "147"));
tabela.insert(make_pair(make_pair(108, "EXP_ARITMETICA4"), "73"));
tabela.insert(make_pair(make_pair(109, ";"), "r41"));
tabela.insert(make_pair(make_pair(109, ")"), "r41"));
tabela.insert(make_pair(make_pair(109, "]"), "r41"));
tabela.insert(make_pair(make_pair(109, "+"), "r41"));
tabela.insert(make_pair(make_pair(109, "-"), "r41"));
tabela.insert(make_pair(make_pair(109, "*"), "r41"));
tabela.insert(make_pair(make_pair(109, "/"), "r41"));
tabela.insert(make_pair(make_pair(109, "%"), "r41"));
tabela.insert(make_pair(make_pair(109, ","), "r41"));
tabela.insert(make_pair(make_pair(109, "=="), "r41"));
tabela.insert(make_pair(make_pair(109, "!="), "r41"));
tabela.insert(make_pair(make_pair(109, ">="), "r41"));
tabela.insert(make_pair(make_pair(109, "<="), "r41"));
tabela.insert(make_pair(make_pair(109, ">"), "r41"));
tabela.insert(make_pair(make_pair(109, "<"), "r41"));
tabela.insert(make_pair(make_pair(109, "ou"), "r41"));
tabela.insert(make_pair(make_pair(109, "e"), "r41"));
tabela.insert(make_pair(make_pair(110, ")"), "s148"));
tabela.insert(make_pair(make_pair(110, "+"), "s104"));
tabela.insert(make_pair(make_pair(110, "-"), "s105"));
tabela.insert(make_pair(make_pair(111, "id"), "s75"));
tabela.insert(make_pair(make_pair(111, "("), "s74"));
tabela.insert(make_pair(make_pair(111, "-"), "s72"));
tabela.insert(make_pair(make_pair(111, "number"), "s76"));
tabela.insert(make_pair(make_pair(111, "char"), "s77"));
tabela.insert(make_pair(make_pair(111, "EXP_ARITMETICA1"), "149"));
tabela.insert(make_pair(make_pair(111, "EXP_ARITMETICA2"), "70"));
tabela.insert(make_pair(make_pair(111, "EXP_ARITMETICA3"), "71"));
tabela.insert(make_pair(make_pair(111, "EXP_ARITMETICA4"), "73"));
tabela.insert(make_pair(make_pair(112, "id"), "s75"));
tabela.insert(make_pair(make_pair(112, "("), "s74"));
tabela.insert(make_pair(make_pair(112, ")"), "s90"));
tabela.insert(make_pair(make_pair(112, "-"), "s72"));
tabela.insert(make_pair(make_pair(112, "number"), "s76"));
tabela.insert(make_pair(make_pair(112, "char"), "s77"));
tabela.insert(make_pair(make_pair(112, "EXP_ARITMETICA1"), "89"));
tabela.insert(make_pair(make_pair(112, "EXP_ARITMETICA2"), "70"));
tabela.insert(make_pair(make_pair(112, "EXP_ARITMETICA3"), "71"));
tabela.insert(make_pair(make_pair(112, "EXP_ARITMETICA4"), "73"));
tabela.insert(make_pair(make_pair(112, "FUNC_ARG1"), "150"));
tabela.insert(make_pair(make_pair(113, ";"), "r75"));
tabela.insert(make_pair(make_pair(113, "{"), "r75"));
tabela.insert(make_pair(make_pair(114, ")"), "s48"));
tabela.insert(make_pair(make_pair(114, ","), "s47"));
tabela.insert(make_pair(make_pair(114, "LISTA_VARS2"), "151"));
tabela.insert(make_pair(make_pair(115, ")"), "s48"));
tabela.insert(make_pair(make_pair(115, "["), "s153"));
tabela.insert(make_pair(make_pair(115, ","), "s47"));
tabela.insert(make_pair(make_pair(115, "LISTA_VARS2"), "152"));
tabela.insert(make_pair(make_pair(116, "int"), "s16"));
tabela.insert(make_pair(make_pair(116, ")"), "s15"));
tabela.insert(make_pair(make_pair(116, "real"), "s17"));
tabela.insert(make_pair(make_pair(116, "letra"), "s18"));
tabela.insert(make_pair(make_pair(116, "TIPO_VAR"), "14"));
tabela.insert(make_pair(make_pair(116, "LISTA_VARS1"), "154"));
tabela.insert(make_pair(make_pair(117, "int"), "r7"));
tabela.insert(make_pair(make_pair(117, "nada"), "r7"));
tabela.insert(make_pair(make_pair(117, "real"), "r7"));
tabela.insert(make_pair(make_pair(117, "letra"), "r7"));
tabela.insert(make_pair(make_pair(117, "$"), "r7"));
tabela.insert(make_pair(make_pair(118, ";"), "s155"));
tabela.insert(make_pair(make_pair(118, "+"), "s104"));
tabela.insert(make_pair(make_pair(118, "-"), "s105"));
tabela.insert(make_pair(make_pair(119, "]"), "s156"));
tabela.insert(make_pair(make_pair(119, "+"), "s104"));
tabela.insert(make_pair(make_pair(119, "-"), "s105"));
tabela.insert(make_pair(make_pair(120, "="), "s157"));
tabela.insert(make_pair(make_pair(121, "id"), "r21"));
tabela.insert(make_pair(make_pair(121, "}"), "r21"));
tabela.insert(make_pair(make_pair(121, "se"), "r21"));
tabela.insert(make_pair(make_pair(121, "uai"), "r21"));
tabela.insert(make_pair(make_pair(121, "le"), "r21"));
tabela.insert(make_pair(make_pair(121, "escreve"), "r21"));
tabela.insert(make_pair(make_pair(121, "tome"), "r21"));
tabela.insert(make_pair(make_pair(121, "vaza"), "r21"));
tabela.insert(make_pair(make_pair(122, "id"), "r30"));
tabela.insert(make_pair(make_pair(122, "}"), "r30"));
tabela.insert(make_pair(make_pair(122, "se"), "r30"));
tabela.insert(make_pair(make_pair(122, "uai"), "r30"));
tabela.insert(make_pair(make_pair(122, "le"), "r30"));
tabela.insert(make_pair(make_pair(122, "escreve"), "r30"));
tabela.insert(make_pair(make_pair(122, "tome"), "r30"));
tabela.insert(make_pair(make_pair(122, "vaza"), "r30"));
tabela.insert(make_pair(make_pair(123, ";"), "r49"));
tabela.insert(make_pair(make_pair(123, ")"), "r49"));
tabela.insert(make_pair(make_pair(123, "]"), "r49"));
tabela.insert(make_pair(make_pair(123, "+"), "r49"));
tabela.insert(make_pair(make_pair(123, "-"), "r49"));
tabela.insert(make_pair(make_pair(123, "*"), "r49"));
tabela.insert(make_pair(make_pair(123, "/"), "r49"));
tabela.insert(make_pair(make_pair(123, "%"), "r49"));
tabela.insert(make_pair(make_pair(123, ","), "r49"));
tabela.insert(make_pair(make_pair(123, "=="), "r49"));
tabela.insert(make_pair(make_pair(123, "!="), "r49"));
tabela.insert(make_pair(make_pair(123, ">="), "r49"));
tabela.insert(make_pair(make_pair(123, "<="), "r49"));
tabela.insert(make_pair(make_pair(123, ">"), "r49"));
tabela.insert(make_pair(make_pair(123, "<"), "r49"));
tabela.insert(make_pair(make_pair(123, "ou"), "r49"));
tabela.insert(make_pair(make_pair(123, "e"), "r49"));
tabela.insert(make_pair(make_pair(124, "id"), "s75"));
tabela.insert(make_pair(make_pair(124, "("), "s74"));
tabela.insert(make_pair(make_pair(124, "-"), "s72"));
tabela.insert(make_pair(make_pair(124, "number"), "s76"));
tabela.insert(make_pair(make_pair(124, "char"), "s77"));
tabela.insert(make_pair(make_pair(124, "EXP_ARITMETICA1"), "158"));
tabela.insert(make_pair(make_pair(124, "EXP_ARITMETICA2"), "70"));
tabela.insert(make_pair(make_pair(124, "EXP_ARITMETICA3"), "71"));
tabela.insert(make_pair(make_pair(124, "EXP_ARITMETICA4"), "73"));
tabela.insert(make_pair(make_pair(125, ";"), "r52"));
tabela.insert(make_pair(make_pair(125, ")"), "r52"));
tabela.insert(make_pair(make_pair(125, "]"), "r52"));
tabela.insert(make_pair(make_pair(125, "+"), "r52"));
tabela.insert(make_pair(make_pair(125, "-"), "r52"));
tabela.insert(make_pair(make_pair(125, "*"), "r52"));
tabela.insert(make_pair(make_pair(125, "/"), "r52"));
tabela.insert(make_pair(make_pair(125, "%"), "r52"));
tabela.insert(make_pair(make_pair(125, ","), "r52"));
tabela.insert(make_pair(make_pair(125, "=="), "r52"));
tabela.insert(make_pair(make_pair(125, "!="), "r52"));
tabela.insert(make_pair(make_pair(125, ">="), "r52"));
tabela.insert(make_pair(make_pair(125, "<="), "r52"));
tabela.insert(make_pair(make_pair(125, ">"), "r52"));
tabela.insert(make_pair(make_pair(125, "<"), "r52"));
tabela.insert(make_pair(make_pair(125, "ou"), "r52"));
tabela.insert(make_pair(make_pair(125, "e"), "r52"));
tabela.insert(make_pair(make_pair(126, "{"), "s159"));
tabela.insert(make_pair(make_pair(127, "id"), "s75"));
tabela.insert(make_pair(make_pair(127, "("), "s96"));
tabela.insert(make_pair(make_pair(127, "-"), "s72"));
tabela.insert(make_pair(make_pair(127, "number"), "s76"));
tabela.insert(make_pair(make_pair(127, "char"), "s77"));
tabela.insert(make_pair(make_pair(127, "nao"), "s94"));
tabela.insert(make_pair(make_pair(127, "EXP_ARITMETICA1"), "98"));
tabela.insert(make_pair(make_pair(127, "EXP_ARITMETICA2"), "70"));
tabela.insert(make_pair(make_pair(127, "EXP_ARITMETICA3"), "71"));
tabela.insert(make_pair(make_pair(127, "EXP_ARITMETICA4"), "73"));
tabela.insert(make_pair(make_pair(127, "EXP_RELACIONAL"), "97"));
tabela.insert(make_pair(make_pair(127, "EXP_COMP2"), "160"));
tabela.insert(make_pair(make_pair(127, "EXP_COMP3"), "93"));
tabela.insert(make_pair(make_pair(127, "EXP_COMP4"), "95"));
tabela.insert(make_pair(make_pair(128, "id"), "s75"));
tabela.insert(make_pair(make_pair(128, "("), "s96"));
tabela.insert(make_pair(make_pair(128, "-"), "s72"));
tabela.insert(make_pair(make_pair(128, "number"), "s76"));
tabela.insert(make_pair(make_pair(128, "char"), "s77"));
tabela.insert(make_pair(make_pair(128, "nao"), "s94"));
tabela.insert(make_pair(make_pair(128, "EXP_ARITMETICA1"), "98"));
tabela.insert(make_pair(make_pair(128, "EXP_ARITMETICA2"), "70"));
tabela.insert(make_pair(make_pair(128, "EXP_ARITMETICA3"), "71"));
tabela.insert(make_pair(make_pair(128, "EXP_ARITMETICA4"), "73"));
tabela.insert(make_pair(make_pair(128, "EXP_RELACIONAL"), "97"));
tabela.insert(make_pair(make_pair(128, "EXP_COMP3"), "161"));
tabela.insert(make_pair(make_pair(128, "EXP_COMP4"), "95"));
tabela.insert(make_pair(make_pair(129, ")"), "r63"));
tabela.insert(make_pair(make_pair(129, "ou"), "r63"));
tabela.insert(make_pair(make_pair(129, "e"), "r63"));
tabela.insert(make_pair(make_pair(130, ")"), "s162"));
tabela.insert(make_pair(make_pair(130, "ou"), "s127"));
tabela.insert(make_pair(make_pair(131, ")"), "s148"));
tabela.insert(make_pair(make_pair(131, "+"), "s104"));
tabela.insert(make_pair(make_pair(131, "-"), "s105"));
tabela.insert(make_pair(make_pair(131, "=="), "s132"));
tabela.insert(make_pair(make_pair(131, "!="), "s133"));
tabela.insert(make_pair(make_pair(131, ">="), "s134"));
tabela.insert(make_pair(make_pair(131, "<="), "s135"));
tabela.insert(make_pair(make_pair(131, ">"), "s136"));
tabela.insert(make_pair(make_pair(131, "<"), "s137"));
tabela.insert(make_pair(make_pair(132, "id"), "s75"));
tabela.insert(make_pair(make_pair(132, "("), "s74"));
tabela.insert(make_pair(make_pair(132, "-"), "s72"));
tabela.insert(make_pair(make_pair(132, "number"), "s76"));
tabela.insert(make_pair(make_pair(132, "char"), "s77"));
tabela.insert(make_pair(make_pair(132, "EXP_ARITMETICA1"), "163"));
tabela.insert(make_pair(make_pair(132, "EXP_ARITMETICA2"), "70"));
tabela.insert(make_pair(make_pair(132, "EXP_ARITMETICA3"), "71"));
tabela.insert(make_pair(make_pair(132, "EXP_ARITMETICA4"), "73"));
tabela.insert(make_pair(make_pair(133, "id"), "s75"));
tabela.insert(make_pair(make_pair(133, "("), "s74"));
tabela.insert(make_pair(make_pair(133, "-"), "s72"));
tabela.insert(make_pair(make_pair(133, "number"), "s76"));
tabela.insert(make_pair(make_pair(133, "char"), "s77"));
tabela.insert(make_pair(make_pair(133, "EXP_ARITMETICA1"), "164"));
tabela.insert(make_pair(make_pair(133, "EXP_ARITMETICA2"), "70"));
tabela.insert(make_pair(make_pair(133, "EXP_ARITMETICA3"), "71"));
tabela.insert(make_pair(make_pair(133, "EXP_ARITMETICA4"), "73"));
tabela.insert(make_pair(make_pair(134, "id"), "s75"));
tabela.insert(make_pair(make_pair(134, "("), "s74"));
tabela.insert(make_pair(make_pair(134, "-"), "s72"));
tabela.insert(make_pair(make_pair(134, "number"), "s76"));
tabela.insert(make_pair(make_pair(134, "char"), "s77"));
tabela.insert(make_pair(make_pair(134, "EXP_ARITMETICA1"), "165"));
tabela.insert(make_pair(make_pair(134, "EXP_ARITMETICA2"), "70"));
tabela.insert(make_pair(make_pair(134, "EXP_ARITMETICA3"), "71"));
tabela.insert(make_pair(make_pair(134, "EXP_ARITMETICA4"), "73"));
tabela.insert(make_pair(make_pair(135, "id"), "s75"));
tabela.insert(make_pair(make_pair(135, "("), "s74"));
tabela.insert(make_pair(make_pair(135, "-"), "s72"));
tabela.insert(make_pair(make_pair(135, "number"), "s76"));
tabela.insert(make_pair(make_pair(135, "char"), "s77"));
tabela.insert(make_pair(make_pair(135, "EXP_ARITMETICA1"), "166"));
tabela.insert(make_pair(make_pair(135, "EXP_ARITMETICA2"), "70"));
tabela.insert(make_pair(make_pair(135, "EXP_ARITMETICA3"), "71"));
tabela.insert(make_pair(make_pair(135, "EXP_ARITMETICA4"), "73"));
tabela.insert(make_pair(make_pair(136, "id"), "s75"));
tabela.insert(make_pair(make_pair(136, "("), "s74"));
tabela.insert(make_pair(make_pair(136, "-"), "s72"));
tabela.insert(make_pair(make_pair(136, "number"), "s76"));
tabela.insert(make_pair(make_pair(136, "char"), "s77"));
tabela.insert(make_pair(make_pair(136, "EXP_ARITMETICA1"), "167"));
tabela.insert(make_pair(make_pair(136, "EXP_ARITMETICA2"), "70"));
tabela.insert(make_pair(make_pair(136, "EXP_ARITMETICA3"), "71"));
tabela.insert(make_pair(make_pair(136, "EXP_ARITMETICA4"), "73"));
tabela.insert(make_pair(make_pair(137, "id"), "s75"));
tabela.insert(make_pair(make_pair(137, "("), "s74"));
tabela.insert(make_pair(make_pair(137, "-"), "s72"));
tabela.insert(make_pair(make_pair(137, "number"), "s76"));
tabela.insert(make_pair(make_pair(137, "char"), "s77"));
tabela.insert(make_pair(make_pair(137, "EXP_ARITMETICA1"), "168"));
tabela.insert(make_pair(make_pair(137, "EXP_ARITMETICA2"), "70"));
tabela.insert(make_pair(make_pair(137, "EXP_ARITMETICA3"), "71"));
tabela.insert(make_pair(make_pair(137, "EXP_ARITMETICA4"), "73"));
tabela.insert(make_pair(make_pair(138, "{"), "s169"));
tabela.insert(make_pair(make_pair(139, ";"), "s170"));
tabela.insert(make_pair(make_pair(140, "id"), "s75"));
tabela.insert(make_pair(make_pair(140, "("), "s74"));
tabela.insert(make_pair(make_pair(140, "-"), "s72"));
tabela.insert(make_pair(make_pair(140, "number"), "s76"));
tabela.insert(make_pair(make_pair(140, "char"), "s77"));
tabela.insert(make_pair(make_pair(140, "EXP_ARITMETICA1"), "171"));
tabela.insert(make_pair(make_pair(140, "EXP_ARITMETICA2"), "70"));
tabela.insert(make_pair(make_pair(140, "EXP_ARITMETICA3"), "71"));
tabela.insert(make_pair(make_pair(140, "EXP_ARITMETICA4"), "73"));
tabela.insert(make_pair(make_pair(141, ";"), "s172"));
tabela.insert(make_pair(make_pair(142, ";"), "s173"));
tabela.insert(make_pair(make_pair(143, ";"), "r34"));
tabela.insert(make_pair(make_pair(143, ")"), "r34"));
tabela.insert(make_pair(make_pair(143, "]"), "r34"));
tabela.insert(make_pair(make_pair(143, "+"), "r34"));
tabela.insert(make_pair(make_pair(143, "-"), "r34"));
tabela.insert(make_pair(make_pair(143, "*"), "s106"));
tabela.insert(make_pair(make_pair(143, "/"), "s107"));
tabela.insert(make_pair(make_pair(143, "%"), "s108"));
tabela.insert(make_pair(make_pair(143, ","), "r34"));
tabela.insert(make_pair(make_pair(143, "=="), "r34"));
tabela.insert(make_pair(make_pair(143, "!="), "r34"));
tabela.insert(make_pair(make_pair(143, ">="), "r34"));
tabela.insert(make_pair(make_pair(143, "<="), "r34"));
tabela.insert(make_pair(make_pair(143, ">"), "r34"));
tabela.insert(make_pair(make_pair(143, "<"), "r34"));
tabela.insert(make_pair(make_pair(143, "ou"), "r34"));
tabela.insert(make_pair(make_pair(143, "e"), "r34"));
tabela.insert(make_pair(make_pair(144, ";"), "r35"));
tabela.insert(make_pair(make_pair(144, ")"), "r35"));
tabela.insert(make_pair(make_pair(144, "]"), "r35"));
tabela.insert(make_pair(make_pair(144, "+"), "r35"));
tabela.insert(make_pair(make_pair(144, "-"), "r35"));
tabela.insert(make_pair(make_pair(144, "*"), "s106"));
tabela.insert(make_pair(make_pair(144, "/"), "s107"));
tabela.insert(make_pair(make_pair(144, "%"), "s108"));
tabela.insert(make_pair(make_pair(144, ","), "r35"));
tabela.insert(make_pair(make_pair(144, "=="), "r35"));
tabela.insert(make_pair(make_pair(144, "!="), "r35"));
tabela.insert(make_pair(make_pair(144, ">="), "r35"));
tabela.insert(make_pair(make_pair(144, "<="), "r35"));
tabela.insert(make_pair(make_pair(144, ">"), "r35"));
tabela.insert(make_pair(make_pair(144, "<"), "r35"));
tabela.insert(make_pair(make_pair(144, "ou"), "r35"));
tabela.insert(make_pair(make_pair(144, "e"), "r35"));
tabela.insert(make_pair(make_pair(145, ";"), "r37"));
tabela.insert(make_pair(make_pair(145, ")"), "r37"));
tabela.insert(make_pair(make_pair(145, "]"), "r37"));
tabela.insert(make_pair(make_pair(145, "+"), "r37"));
tabela.insert(make_pair(make_pair(145, "-"), "r37"));
tabela.insert(make_pair(make_pair(145, "*"), "r37"));
tabela.insert(make_pair(make_pair(145, "/"), "r37"));
tabela.insert(make_pair(make_pair(145, "%"), "r37"));
tabela.insert(make_pair(make_pair(145, ","), "r37"));
tabela.insert(make_pair(make_pair(145, "=="), "r37"));
tabela.insert(make_pair(make_pair(145, "!="), "r37"));
tabela.insert(make_pair(make_pair(145, ">="), "r37"));
tabela.insert(make_pair(make_pair(145, "<="), "r37"));
tabela.insert(make_pair(make_pair(145, ">"), "r37"));
tabela.insert(make_pair(make_pair(145, "<"), "r37"));
tabela.insert(make_pair(make_pair(145, "ou"), "r37"));
tabela.insert(make_pair(make_pair(145, "e"), "r37"));
tabela.insert(make_pair(make_pair(146, ";"), "r38"));
tabela.insert(make_pair(make_pair(146, ")"), "r38"));
tabela.insert(make_pair(make_pair(146, "]"), "r38"));
tabela.insert(make_pair(make_pair(146, "+"), "r38"));
tabela.insert(make_pair(make_pair(146, "-"), "r38"));
tabela.insert(make_pair(make_pair(146, "*"), "r38"));
tabela.insert(make_pair(make_pair(146, "/"), "r38"));
tabela.insert(make_pair(make_pair(146, "%"), "r38"));
tabela.insert(make_pair(make_pair(146, ","), "r38"));
tabela.insert(make_pair(make_pair(146, "=="), "r38"));
tabela.insert(make_pair(make_pair(146, "!="), "r38"));
tabela.insert(make_pair(make_pair(146, ">="), "r38"));
tabela.insert(make_pair(make_pair(146, "<="), "r38"));
tabela.insert(make_pair(make_pair(146, ">"), "r38"));
tabela.insert(make_pair(make_pair(146, "<"), "r38"));
tabela.insert(make_pair(make_pair(146, "ou"), "r38"));
tabela.insert(make_pair(make_pair(146, "e"), "r38"));
tabela.insert(make_pair(make_pair(147, ";"), "r39"));
tabela.insert(make_pair(make_pair(147, ")"), "r39"));
tabela.insert(make_pair(make_pair(147, "]"), "r39"));
tabela.insert(make_pair(make_pair(147, "+"), "r39"));
tabela.insert(make_pair(make_pair(147, "-"), "r39"));
tabela.insert(make_pair(make_pair(147, "*"), "r39"));
tabela.insert(make_pair(make_pair(147, "/"), "r39"));
tabela.insert(make_pair(make_pair(147, "%"), "r39"));
tabela.insert(make_pair(make_pair(147, ","), "r39"));
tabela.insert(make_pair(make_pair(147, "=="), "r39"));
tabela.insert(make_pair(make_pair(147, "!="), "r39"));
tabela.insert(make_pair(make_pair(147, ">="), "r39"));
tabela.insert(make_pair(make_pair(147, "<="), "r39"));
tabela.insert(make_pair(make_pair(147, ">"), "r39"));
tabela.insert(make_pair(make_pair(147, "<"), "r39"));
tabela.insert(make_pair(make_pair(147, "ou"), "r39"));
tabela.insert(make_pair(make_pair(147, "e"), "r39"));
tabela.insert(make_pair(make_pair(148, ";"), "r43"));
tabela.insert(make_pair(make_pair(148, ")"), "r43"));
tabela.insert(make_pair(make_pair(148, "]"), "r43"));
tabela.insert(make_pair(make_pair(148, "+"), "r43"));
tabela.insert(make_pair(make_pair(148, "-"), "r43"));
tabela.insert(make_pair(make_pair(148, "*"), "r43"));
tabela.insert(make_pair(make_pair(148, "/"), "r43"));
tabela.insert(make_pair(make_pair(148, "%"), "r43"));
tabela.insert(make_pair(make_pair(148, ","), "r43"));
tabela.insert(make_pair(make_pair(148, "=="), "r43"));
tabela.insert(make_pair(make_pair(148, "!="), "r43"));
tabela.insert(make_pair(make_pair(148, ">="), "r43"));
tabela.insert(make_pair(make_pair(148, "<="), "r43"));
tabela.insert(make_pair(make_pair(148, ">"), "r43"));
tabela.insert(make_pair(make_pair(148, "<"), "r43"));
tabela.insert(make_pair(make_pair(148, "ou"), "r43"));
tabela.insert(make_pair(make_pair(148, "e"), "r43"));
tabela.insert(make_pair(make_pair(149, "]"), "s174"));
tabela.insert(make_pair(make_pair(149, "+"), "s104"));
tabela.insert(make_pair(make_pair(149, "-"), "s105"));
tabela.insert(make_pair(make_pair(150, ";"), "r48"));
tabela.insert(make_pair(make_pair(150, ")"), "r48"));
tabela.insert(make_pair(make_pair(150, "]"), "r48"));
tabela.insert(make_pair(make_pair(150, "+"), "r48"));
tabela.insert(make_pair(make_pair(150, "-"), "r48"));
tabela.insert(make_pair(make_pair(150, "*"), "r48"));
tabela.insert(make_pair(make_pair(150, "/"), "r48"));
tabela.insert(make_pair(make_pair(150, "%"), "r48"));
tabela.insert(make_pair(make_pair(150, ","), "r48"));
tabela.insert(make_pair(make_pair(150, "=="), "r48"));
tabela.insert(make_pair(make_pair(150, "!="), "r48"));
tabela.insert(make_pair(make_pair(150, ">="), "r48"));
tabela.insert(make_pair(make_pair(150, "<="), "r48"));
tabela.insert(make_pair(make_pair(150, ">"), "r48"));
tabela.insert(make_pair(make_pair(150, "<"), "r48"));
tabela.insert(make_pair(make_pair(150, "ou"), "r48"));
tabela.insert(make_pair(make_pair(150, "e"), "r48"));
tabela.insert(make_pair(make_pair(151, ";"), "r76"));
tabela.insert(make_pair(make_pair(151, "{"), "r76"));
tabela.insert(make_pair(make_pair(152, ";"), "r78"));
tabela.insert(make_pair(make_pair(152, "{"), "r78"));
tabela.insert(make_pair(make_pair(153, "id"), "s75"));
tabela.insert(make_pair(make_pair(153, "("), "s74"));
tabela.insert(make_pair(make_pair(153, "]"), "s175"));
tabela.insert(make_pair(make_pair(153, "-"), "s72"));
tabela.insert(make_pair(make_pair(153, "number"), "s76"));
tabela.insert(make_pair(make_pair(153, "char"), "s77"));
tabela.insert(make_pair(make_pair(153, "EXP_ARITMETICA1"), "176"));
tabela.insert(make_pair(make_pair(153, "EXP_ARITMETICA2"), "70"));
tabela.insert(make_pair(make_pair(153, "EXP_ARITMETICA3"), "71"));
tabela.insert(make_pair(make_pair(153, "EXP_ARITMETICA4"), "73"));
tabela.insert(make_pair(make_pair(154, "{"), "s21"));
tabela.insert(make_pair(make_pair(155, "id"), "s38"));
tabela.insert(make_pair(make_pair(155, "int"), "s16"));
tabela.insert(make_pair(make_pair(155, "}"), "s37"));
tabela.insert(make_pair(make_pair(155, "se"), "s39"));
tabela.insert(make_pair(make_pair(155, "uai"), "s40"));
tabela.insert(make_pair(make_pair(155, "le"), "s41"));
tabela.insert(make_pair(make_pair(155, "escreve"), "s42"));
tabela.insert(make_pair(make_pair(155, "tome"), "s43"));
tabela.insert(make_pair(make_pair(155, "vaza"), "s44"));
tabela.insert(make_pair(make_pair(155, "real"), "s17"));
tabela.insert(make_pair(make_pair(155, "letra"), "s18"));
tabela.insert(make_pair(make_pair(155, "DECLARACAO"), "177"));
tabela.insert(make_pair(make_pair(155, "BLOCO"), "28"));
tabela.insert(make_pair(make_pair(155, "ATRIBUICAO"), "29"));
tabela.insert(make_pair(make_pair(155, "DESVIO_CONDICIONAL"), "30"));
tabela.insert(make_pair(make_pair(155, "LOOP"), "31"));
tabela.insert(make_pair(make_pair(155, "LEITURA"), "32"));
tabela.insert(make_pair(make_pair(155, "ESCRITA"), "33"));
tabela.insert(make_pair(make_pair(155, "FUNC_NADA"), "34"));
tabela.insert(make_pair(make_pair(155, "TOME"), "35"));
tabela.insert(make_pair(make_pair(155, "VAZA"), "36"));
tabela.insert(make_pair(make_pair(155, "TIPO_VAR"), "27"));
tabela.insert(make_pair(make_pair(156, ";"), "s178"));
tabela.insert(make_pair(make_pair(157, "id"), "s75"));
tabela.insert(make_pair(make_pair(157, "("), "s74"));
tabela.insert(make_pair(make_pair(157, "-"), "s72"));
tabela.insert(make_pair(make_pair(157, "number"), "s76"));
tabela.insert(make_pair(make_pair(157, "char"), "s77"));
tabela.insert(make_pair(make_pair(157, "EXP_ARITMETICA1"), "179"));
tabela.insert(make_pair(make_pair(157, "EXP_ARITMETICA2"), "70"));
tabela.insert(make_pair(make_pair(157, "EXP_ARITMETICA3"), "71"));
tabela.insert(make_pair(make_pair(157, "EXP_ARITMETICA4"), "73"));
tabela.insert(make_pair(make_pair(158, ")"), "s125"));
tabela.insert(make_pair(make_pair(158, "+"), "s104"));
tabela.insert(make_pair(make_pair(158, "-"), "s105"));
tabela.insert(make_pair(make_pair(158, ","), "s124"));
tabela.insert(make_pair(make_pair(158, "FUNC_ARG2"), "180"));
tabela.insert(make_pair(make_pair(159, "id"), "s38"));
tabela.insert(make_pair(make_pair(159, "}"), "s37"));
tabela.insert(make_pair(make_pair(159, "se"), "s39"));
tabela.insert(make_pair(make_pair(159, "uai"), "s40"));
tabela.insert(make_pair(make_pair(159, "le"), "s41"));
tabela.insert(make_pair(make_pair(159, "escreve"), "s42"));
tabela.insert(make_pair(make_pair(159, "tome"), "s43"));
tabela.insert(make_pair(make_pair(159, "vaza"), "s44"));
tabela.insert(make_pair(make_pair(159, "BLOCO"), "182"));
tabela.insert(make_pair(make_pair(159, "ATRIBUICAO"), "29"));
tabela.insert(make_pair(make_pair(159, "DESVIO_CONDICIONAL"), "30"));
tabela.insert(make_pair(make_pair(159, "DESVIO_CONDICIONAL2"), "181"));
tabela.insert(make_pair(make_pair(159, "LOOP"), "31"));
tabela.insert(make_pair(make_pair(159, "LEITURA"), "32"));
tabela.insert(make_pair(make_pair(159, "ESCRITA"), "33"));
tabela.insert(make_pair(make_pair(159, "FUNC_NADA"), "34"));
tabela.insert(make_pair(make_pair(159, "TOME"), "35"));
tabela.insert(make_pair(make_pair(159, "VAZA"), "36"));
tabela.insert(make_pair(make_pair(160, ")"), "r59"));
tabela.insert(make_pair(make_pair(160, "ou"), "r59"));
tabela.insert(make_pair(make_pair(160, "e"), "s128"));
tabela.insert(make_pair(make_pair(161, ")"), "r61"));
tabela.insert(make_pair(make_pair(161, "ou"), "r61"));
tabela.insert(make_pair(make_pair(161, "e"), "r61"));
tabela.insert(make_pair(make_pair(162, ")"), "r65"));
tabela.insert(make_pair(make_pair(162, "ou"), "r65"));
tabela.insert(make_pair(make_pair(162, "e"), "r65"));
tabela.insert(make_pair(make_pair(163, ")"), "r53"));
tabela.insert(make_pair(make_pair(163, "+"), "s104"));
tabela.insert(make_pair(make_pair(163, "-"), "s105"));
tabela.insert(make_pair(make_pair(163, "ou"), "r53"));
tabela.insert(make_pair(make_pair(163, "e"), "r53"));
tabela.insert(make_pair(make_pair(164, ")"), "r54"));
tabela.insert(make_pair(make_pair(164, "+"), "s104"));
tabela.insert(make_pair(make_pair(164, "-"), "s105"));
tabela.insert(make_pair(make_pair(164, "ou"), "r54"));
tabela.insert(make_pair(make_pair(164, "e"), "r54"));
tabela.insert(make_pair(make_pair(165, ")"), "r55"));
tabela.insert(make_pair(make_pair(165, "+"), "s104"));
tabela.insert(make_pair(make_pair(165, "-"), "s105"));
tabela.insert(make_pair(make_pair(165, "ou"), "r55"));
tabela.insert(make_pair(make_pair(165, "e"), "r55"));
tabela.insert(make_pair(make_pair(166, ")"), "r56"));
tabela.insert(make_pair(make_pair(166, "+"), "s104"));
tabela.insert(make_pair(make_pair(166, "-"), "s105"));
tabela.insert(make_pair(make_pair(166, "ou"), "r56"));
tabela.insert(make_pair(make_pair(166, "e"), "r56"));
tabela.insert(make_pair(make_pair(167, ")"), "r57"));
tabela.insert(make_pair(make_pair(167, "+"), "s104"));
tabela.insert(make_pair(make_pair(167, "-"), "s105"));
tabela.insert(make_pair(make_pair(167, "ou"), "r57"));
tabela.insert(make_pair(make_pair(167, "e"), "r57"));
tabela.insert(make_pair(make_pair(168, ")"), "r58"));
tabela.insert(make_pair(make_pair(168, "+"), "s104"));
tabela.insert(make_pair(make_pair(168, "-"), "s105"));
tabela.insert(make_pair(make_pair(168, "ou"), "r58"));
tabela.insert(make_pair(make_pair(168, "e"), "r58"));
tabela.insert(make_pair(make_pair(169, "id"), "s38"));
tabela.insert(make_pair(make_pair(169, "}"), "s37"));
tabela.insert(make_pair(make_pair(169, "se"), "s39"));
tabela.insert(make_pair(make_pair(169, "uai"), "s40"));
tabela.insert(make_pair(make_pair(169, "le"), "s41"));
tabela.insert(make_pair(make_pair(169, "escreve"), "s42"));
tabela.insert(make_pair(make_pair(169, "tome"), "s43"));
tabela.insert(make_pair(make_pair(169, "vaza"), "s44"));
tabela.insert(make_pair(make_pair(169, "BLOCO"), "183"));
tabela.insert(make_pair(make_pair(169, "ATRIBUICAO"), "29"));
tabela.insert(make_pair(make_pair(169, "DESVIO_CONDICIONAL"), "30"));
tabela.insert(make_pair(make_pair(169, "LOOP"), "31"));
tabela.insert(make_pair(make_pair(169, "LEITURA"), "32"));
tabela.insert(make_pair(make_pair(169, "ESCRITA"), "33"));
tabela.insert(make_pair(make_pair(169, "FUNC_NADA"), "34"));
tabela.insert(make_pair(make_pair(169, "TOME"), "35"));
tabela.insert(make_pair(make_pair(169, "VAZA"), "36"));
tabela.insert(make_pair(make_pair(170, "id"), "r26"));
tabela.insert(make_pair(make_pair(170, "}"), "r26"));
tabela.insert(make_pair(make_pair(170, "se"), "r26"));
tabela.insert(make_pair(make_pair(170, "uai"), "r26"));
tabela.insert(make_pair(make_pair(170, "le"), "r26"));
tabela.insert(make_pair(make_pair(170, "escreve"), "r26"));
tabela.insert(make_pair(make_pair(170, "tome"), "r26"));
tabela.insert(make_pair(make_pair(170, "vaza"), "r26"));
tabela.insert(make_pair(make_pair(171, "]"), "s184"));
tabela.insert(make_pair(make_pair(171, "+"), "s104"));
tabela.insert(make_pair(make_pair(171, "-"), "s105"));
tabela.insert(make_pair(make_pair(172, "id"), "r28"));
tabela.insert(make_pair(make_pair(172, "}"), "r28"));
tabela.insert(make_pair(make_pair(172, "se"), "r28"));
tabela.insert(make_pair(make_pair(172, "uai"), "r28"));
tabela.insert(make_pair(make_pair(172, "le"), "r28"));
tabela.insert(make_pair(make_pair(172, "escreve"), "r28"));
tabela.insert(make_pair(make_pair(172, "tome"), "r28"));
tabela.insert(make_pair(make_pair(172, "vaza"), "r28"));
tabela.insert(make_pair(make_pair(173, "id"), "r29"));
tabela.insert(make_pair(make_pair(173, "}"), "r29"));
tabela.insert(make_pair(make_pair(173, "se"), "r29"));
tabela.insert(make_pair(make_pair(173, "uai"), "r29"));
tabela.insert(make_pair(make_pair(173, "le"), "r29"));
tabela.insert(make_pair(make_pair(173, "escreve"), "r29"));
tabela.insert(make_pair(make_pair(173, "tome"), "r29"));
tabela.insert(make_pair(make_pair(173, "vaza"), "r29"));
tabela.insert(make_pair(make_pair(174, ";"), "r47"));
tabela.insert(make_pair(make_pair(174, ")"), "r47"));
tabela.insert(make_pair(make_pair(174, "]"), "r47"));
tabela.insert(make_pair(make_pair(174, "+"), "r47"));
tabela.insert(make_pair(make_pair(174, "-"), "r47"));
tabela.insert(make_pair(make_pair(174, "*"), "r47"));
tabela.insert(make_pair(make_pair(174, "/"), "r47"));
tabela.insert(make_pair(make_pair(174, "%"), "r47"));
tabela.insert(make_pair(make_pair(174, ","), "r47"));
tabela.insert(make_pair(make_pair(174, "=="), "r47"));
tabela.insert(make_pair(make_pair(174, "!="), "r47"));
tabela.insert(make_pair(make_pair(174, ">="), "r47"));
tabela.insert(make_pair(make_pair(174, "<="), "r47"));
tabela.insert(make_pair(make_pair(174, ">"), "r47"));
tabela.insert(make_pair(make_pair(174, "<"), "r47"));
tabela.insert(make_pair(make_pair(174, "ou"), "r47"));
tabela.insert(make_pair(make_pair(174, "e"), "r47"));
tabela.insert(make_pair(make_pair(175, ")"), "s48"));
tabela.insert(make_pair(make_pair(175, ","), "s47"));
tabela.insert(make_pair(make_pair(175, "LISTA_VARS2"), "185"));
tabela.insert(make_pair(make_pair(176, "]"), "s186"));
tabela.insert(make_pair(make_pair(176, "+"), "s104"));
tabela.insert(make_pair(make_pair(176, "-"), "s105"));
tabela.insert(make_pair(make_pair(177, "int"), "r8"));
tabela.insert(make_pair(make_pair(177, "nada"), "r8"));
tabela.insert(make_pair(make_pair(177, "real"), "r8"));
tabela.insert(make_pair(make_pair(177, "letra"), "r8"));
tabela.insert(make_pair(make_pair(177, "$"), "r8"));
tabela.insert(make_pair(make_pair(178, "id"), "s38"));
tabela.insert(make_pair(make_pair(178, "int"), "s16"));
tabela.insert(make_pair(make_pair(178, "}"), "s37"));
tabela.insert(make_pair(make_pair(178, "se"), "s39"));
tabela.insert(make_pair(make_pair(178, "uai"), "s40"));
tabela.insert(make_pair(make_pair(178, "le"), "s41"));
tabela.insert(make_pair(make_pair(178, "escreve"), "s42"));
tabela.insert(make_pair(make_pair(178, "tome"), "s43"));
tabela.insert(make_pair(make_pair(178, "vaza"), "s44"));
tabela.insert(make_pair(make_pair(178, "real"), "s17"));
tabela.insert(make_pair(make_pair(178, "letra"), "s18"));
tabela.insert(make_pair(make_pair(178, "DECLARACAO"), "187"));
tabela.insert(make_pair(make_pair(178, "BLOCO"), "28"));
tabela.insert(make_pair(make_pair(178, "ATRIBUICAO"), "29"));
tabela.insert(make_pair(make_pair(178, "DESVIO_CONDICIONAL"), "30"));
tabela.insert(make_pair(make_pair(178, "LOOP"), "31"));
tabela.insert(make_pair(make_pair(178, "LEITURA"), "32"));
tabela.insert(make_pair(make_pair(178, "ESCRITA"), "33"));
tabela.insert(make_pair(make_pair(178, "FUNC_NADA"), "34"));
tabela.insert(make_pair(make_pair(178, "TOME"), "35"));
tabela.insert(make_pair(make_pair(178, "VAZA"), "36"));
tabela.insert(make_pair(make_pair(178, "TIPO_VAR"), "27"));
tabela.insert(make_pair(make_pair(179, ";"), "s188"));
tabela.insert(make_pair(make_pair(179, "+"), "s104"));
tabela.insert(make_pair(make_pair(179, "-"), "s105"));
tabela.insert(make_pair(make_pair(180, ";"), "r51"));
tabela.insert(make_pair(make_pair(180, ")"), "r51"));
tabela.insert(make_pair(make_pair(180, "]"), "r51"));
tabela.insert(make_pair(make_pair(180, "+"), "r51"));
tabela.insert(make_pair(make_pair(180, "-"), "r51"));
tabela.insert(make_pair(make_pair(180, "*"), "r51"));
tabela.insert(make_pair(make_pair(180, "/"), "r51"));
tabela.insert(make_pair(make_pair(180, "%"), "r51"));
tabela.insert(make_pair(make_pair(180, ","), "r51"));
tabela.insert(make_pair(make_pair(180, "=="), "r51"));
tabela.insert(make_pair(make_pair(180, "!="), "r51"));
tabela.insert(make_pair(make_pair(180, ">="), "r51"));
tabela.insert(make_pair(make_pair(180, "<="), "r51"));
tabela.insert(make_pair(make_pair(180, ">"), "r51"));
tabela.insert(make_pair(make_pair(180, "<"), "r51"));
tabela.insert(make_pair(make_pair(180, "ou"), "r51"));
tabela.insert(make_pair(make_pair(180, "e"), "r51"));
tabela.insert(make_pair(make_pair(181, "id"), "r22"));
tabela.insert(make_pair(make_pair(181, "}"), "r22"));
tabela.insert(make_pair(make_pair(181, "se"), "r22"));
tabela.insert(make_pair(make_pair(181, "uai"), "r22"));
tabela.insert(make_pair(make_pair(181, "le"), "r22"));
tabela.insert(make_pair(make_pair(181, "escreve"), "r22"));
tabela.insert(make_pair(make_pair(181, "tome"), "r22"));
tabela.insert(make_pair(make_pair(181, "vaza"), "r22"));
tabela.insert(make_pair(make_pair(182, "id"), "r24"));
tabela.insert(make_pair(make_pair(182, "}"), "r24"));
tabela.insert(make_pair(make_pair(182, "se"), "r24"));
tabela.insert(make_pair(make_pair(182, "senao"), "s189"));
tabela.insert(make_pair(make_pair(182, "uai"), "r24"));
tabela.insert(make_pair(make_pair(182, "le"), "r24"));
tabela.insert(make_pair(make_pair(182, "escreve"), "r24"));
tabela.insert(make_pair(make_pair(182, "tome"), "r24"));
tabela.insert(make_pair(make_pair(182, "vaza"), "r24"));
tabela.insert(make_pair(make_pair(183, "id"), "r25"));
tabela.insert(make_pair(make_pair(183, "}"), "r25"));
tabela.insert(make_pair(make_pair(183, "se"), "r25"));
tabela.insert(make_pair(make_pair(183, "uai"), "r25"));
tabela.insert(make_pair(make_pair(183, "le"), "r25"));
tabela.insert(make_pair(make_pair(183, "escreve"), "r25"));
tabela.insert(make_pair(make_pair(183, "tome"), "r25"));
tabela.insert(make_pair(make_pair(183, "vaza"), "r25"));
tabela.insert(make_pair(make_pair(184, ")"), "s190"));
tabela.insert(make_pair(make_pair(185, ";"), "r79"));
tabela.insert(make_pair(make_pair(185, "{"), "r79"));
tabela.insert(make_pair(make_pair(186, ")"), "s48"));
tabela.insert(make_pair(make_pair(186, ","), "s47"));
tabela.insert(make_pair(make_pair(186, "LISTA_VARS2"), "191"));
tabela.insert(make_pair(make_pair(187, "int"), "r9"));
tabela.insert(make_pair(make_pair(187, "nada"), "r9"));
tabela.insert(make_pair(make_pair(187, "real"), "r9"));
tabela.insert(make_pair(make_pair(187, "letra"), "r9"));
tabela.insert(make_pair(make_pair(187, "$"), "r9"));
tabela.insert(make_pair(make_pair(188, "id"), "r20"));
tabela.insert(make_pair(make_pair(188, "}"), "r20"));
tabela.insert(make_pair(make_pair(188, "se"), "r20"));
tabela.insert(make_pair(make_pair(188, "uai"), "r20"));
tabela.insert(make_pair(make_pair(188, "le"), "r20"));
tabela.insert(make_pair(make_pair(188, "escreve"), "r20"));
tabela.insert(make_pair(make_pair(188, "tome"), "r20"));
tabela.insert(make_pair(make_pair(188, "vaza"), "r20"));
tabela.insert(make_pair(make_pair(189, "{"), "s192"));
tabela.insert(make_pair(make_pair(190, ";"), "s193"));
tabela.insert(make_pair(make_pair(191, ";"), "r80"));
tabela.insert(make_pair(make_pair(191, "{"), "r80"));
tabela.insert(make_pair(make_pair(192, "id"), "s38"));
tabela.insert(make_pair(make_pair(192, "}"), "s37"));
tabela.insert(make_pair(make_pair(192, "se"), "s39"));
tabela.insert(make_pair(make_pair(192, "uai"), "s40"));
tabela.insert(make_pair(make_pair(192, "le"), "s41"));
tabela.insert(make_pair(make_pair(192, "escreve"), "s42"));
tabela.insert(make_pair(make_pair(192, "tome"), "s43"));
tabela.insert(make_pair(make_pair(192, "vaza"), "s44"));
tabela.insert(make_pair(make_pair(192, "BLOCO"), "194"));
tabela.insert(make_pair(make_pair(192, "ATRIBUICAO"), "29"));
tabela.insert(make_pair(make_pair(192, "DESVIO_CONDICIONAL"), "30"));
tabela.insert(make_pair(make_pair(192, "LOOP"), "31"));
tabela.insert(make_pair(make_pair(192, "LEITURA"), "32"));
tabela.insert(make_pair(make_pair(192, "ESCRITA"), "33"));
tabela.insert(make_pair(make_pair(192, "FUNC_NADA"), "34"));
tabela.insert(make_pair(make_pair(192, "TOME"), "35"));
tabela.insert(make_pair(make_pair(192, "VAZA"), "36"));
tabela.insert(make_pair(make_pair(193, "id"), "r27"));
tabela.insert(make_pair(make_pair(193, "}"), "r27"));
tabela.insert(make_pair(make_pair(193, "se"), "r27"));
tabela.insert(make_pair(make_pair(193, "uai"), "r27"));
tabela.insert(make_pair(make_pair(193, "le"), "r27"));
tabela.insert(make_pair(make_pair(193, "escreve"), "r27"));
tabela.insert(make_pair(make_pair(193, "tome"), "r27"));
tabela.insert(make_pair(make_pair(193, "vaza"), "r27"));
tabela.insert(make_pair(make_pair(194, "id"), "r23"));
tabela.insert(make_pair(make_pair(194, "}"), "r23"));
tabela.insert(make_pair(make_pair(194, "se"), "r23"));
tabela.insert(make_pair(make_pair(194, "uai"), "r23"));
tabela.insert(make_pair(make_pair(194, "le"), "r23"));
tabela.insert(make_pair(make_pair(194, "escreve"), "r23"));
tabela.insert(make_pair(make_pair(194, "tome"), "r23"));
tabela.insert(make_pair(make_pair(194, "vaza"), "r23"));
}

void preenche_transicoes(map <int, pair<int,string>> &transicoes){
transicoes.insert(make_pair(0,make_pair(1,"INICIO")));
transicoes.insert(make_pair(1,make_pair(6,"PROTOTIPO_FUNC")));
transicoes.insert(make_pair(2,make_pair(1,"PROTOTIPO_FUNC")));
transicoes.insert(make_pair(3,make_pair(7,"FUNC")));
transicoes.insert(make_pair(4,make_pair(1,"FUNC")));
transicoes.insert(make_pair(5,make_pair(6,"INT_MAINHA")));
transicoes.insert(make_pair(6,make_pair(1,"BLOCO_MAIN")));
transicoes.insert(make_pair(7,make_pair(4,"DECLARACAO")));
transicoes.insert(make_pair(8,make_pair(6,"DECLARACAO")));
transicoes.insert(make_pair(9,make_pair(7,"DECLARACAO")));
transicoes.insert(make_pair(10,make_pair(1,"DECLARACAO")));
transicoes.insert(make_pair(11,make_pair(2,"BLOCO")));
transicoes.insert(make_pair(12,make_pair(2,"BLOCO")));
transicoes.insert(make_pair(13,make_pair(2,"BLOCO")));
transicoes.insert(make_pair(14,make_pair(2,"BLOCO")));
transicoes.insert(make_pair(15,make_pair(2,"BLOCO")));
transicoes.insert(make_pair(16,make_pair(2,"BLOCO")));
transicoes.insert(make_pair(17,make_pair(2,"BLOCO")));
transicoes.insert(make_pair(18,make_pair(2,"BLOCO")));
transicoes.insert(make_pair(19,make_pair(1,"BLOCO")));
transicoes.insert(make_pair(20,make_pair(7,"ATRIBUICAO")));
transicoes.insert(make_pair(21,make_pair(4,"ATRIBUICAO")));
transicoes.insert(make_pair(22,make_pair(6,"DESVIO_CONDICIONAL")));
transicoes.insert(make_pair(23,make_pair(4,"DESVIO_CONDICIONAL2")));
transicoes.insert(make_pair(24,make_pair(1,"DESVIO_CONDICIONAL2")));
transicoes.insert(make_pair(25,make_pair(6,"LOOP")));
transicoes.insert(make_pair(26,make_pair(5,"LEITURA")));
transicoes.insert(make_pair(27,make_pair(8,"LEITURA")));
transicoes.insert(make_pair(28,make_pair(5,"ESCRITA")));
transicoes.insert(make_pair(29,make_pair(5,"ESCRITA")));
transicoes.insert(make_pair(30,make_pair(4,"FUNC_NADA")));
transicoes.insert(make_pair(31,make_pair(2,"TOME")));
transicoes.insert(make_pair(32,make_pair(3,"TOME")));
transicoes.insert(make_pair(33,make_pair(2,"VAZA")));
transicoes.insert(make_pair(34,make_pair(3,"EXP_ARITMETICA1")));
transicoes.insert(make_pair(35,make_pair(3,"EXP_ARITMETICA1")));
transicoes.insert(make_pair(36,make_pair(1,"EXP_ARITMETICA1")));
transicoes.insert(make_pair(37,make_pair(3,"EXP_ARITMETICA2")));
transicoes.insert(make_pair(38,make_pair(3,"EXP_ARITMETICA2")));
transicoes.insert(make_pair(39,make_pair(3,"EXP_ARITMETICA2")));
transicoes.insert(make_pair(40,make_pair(1,"EXP_ARITMETICA2")));
transicoes.insert(make_pair(41,make_pair(2,"EXP_ARITMETICA3")));
transicoes.insert(make_pair(42,make_pair(1,"EXP_ARITMETICA3")));
transicoes.insert(make_pair(43,make_pair(3,"EXP_ARITMETICA4")));
transicoes.insert(make_pair(44,make_pair(1,"EXP_ARITMETICA4")));
transicoes.insert(make_pair(45,make_pair(1,"EXP_ARITMETICA4")));
transicoes.insert(make_pair(46,make_pair(1,"EXP_ARITMETICA4")));
transicoes.insert(make_pair(47,make_pair(4,"EXP_ARITMETICA4")));
transicoes.insert(make_pair(48,make_pair(3,"EXP_ARITMETICA4")));
transicoes.insert(make_pair(49,make_pair(2,"FUNC_ARG1")));
transicoes.insert(make_pair(50,make_pair(1,"FUNC_ARG1")));
transicoes.insert(make_pair(51,make_pair(3,"FUNC_ARG2")));
transicoes.insert(make_pair(52,make_pair(1,"FUNC_ARG2")));
transicoes.insert(make_pair(53,make_pair(3,"EXP_RELACIONAL")));
transicoes.insert(make_pair(54,make_pair(3,"EXP_RELACIONAL")));
transicoes.insert(make_pair(55,make_pair(3,"EXP_RELACIONAL")));
transicoes.insert(make_pair(56,make_pair(3,"EXP_RELACIONAL")));
transicoes.insert(make_pair(57,make_pair(3,"EXP_RELACIONAL")));
transicoes.insert(make_pair(58,make_pair(3,"EXP_RELACIONAL")));
transicoes.insert(make_pair(59,make_pair(3,"EXP_COMP1")));
transicoes.insert(make_pair(60,make_pair(1,"EXP_COMP1")));
transicoes.insert(make_pair(61,make_pair(3,"EXP_COMP2")));
transicoes.insert(make_pair(62,make_pair(1,"EXP_COMP2")));
transicoes.insert(make_pair(63,make_pair(2,"EXP_COMP3")));
transicoes.insert(make_pair(64,make_pair(1,"EXP_COMP3")));
transicoes.insert(make_pair(65,make_pair(3,"EXP_COMP4")));
transicoes.insert(make_pair(66,make_pair(1,"EXP_COMP4")));
transicoes.insert(make_pair(67,make_pair(1,"TIPO_FUNC")));
transicoes.insert(make_pair(68,make_pair(1,"TIPO_FUNC")));
transicoes.insert(make_pair(69,make_pair(1,"TIPO_FUNC")));
transicoes.insert(make_pair(70,make_pair(1,"TIPO_FUNC")));
transicoes.insert(make_pair(71,make_pair(1,"TIPO_VAR")));
transicoes.insert(make_pair(72,make_pair(1,"TIPO_VAR")));
transicoes.insert(make_pair(73,make_pair(1,"TIPO_VAR")));
transicoes.insert(make_pair(74,make_pair(3,"LISTA_VARS1")));
transicoes.insert(make_pair(75,make_pair(5,"LISTA_VARS1")));
transicoes.insert(make_pair(76,make_pair(6,"LISTA_VARS1")));
transicoes.insert(make_pair(77,make_pair(1,"LISTA_VARS1")));
transicoes.insert(make_pair(78,make_pair(4,"LISTA_VARS2")));
transicoes.insert(make_pair(79,make_pair(6,"LISTA_VARS2")));
transicoes.insert(make_pair(80,make_pair(7,"LISTA_VARS2")));
transicoes.insert(make_pair(81,make_pair(1,"LISTA_VARS2")));
}

void preencheCabecaRegra(set <string> &regras){
	regras.insert("INICIO");
	regras.insert("PROTOTIPO_FUNC");
	regras.insert("FUNC");
	regras.insert("INT_MAINHA");
	regras.insert("BLOCO_MAIN");
	regras.insert("DECLARACAO");
	regras.insert("BLOCO");
	regras.insert("ATRIBUICAO");
	regras.insert("DESVIO_CONDICIONAL");
	regras.insert("LOOP");
	regras.insert("LEITURA");
	regras.insert("ESCRITA");
	regras.insert("FUNC_NADA");
	regras.insert("TOME");
	regras.insert("VAZA");
	regras.insert("EXP_ARITMETICA1");
	regras.insert("EXP_ARITMETICA2");
	regras.insert("EXP_ARITMETICA3");
	regras.insert("EXP_ARITMETICA4");
	regras.insert("FUNC_ARG1");
	regras.insert("FUNC_ARG2");
	regras.insert("EXP_RELACIONAL");
	regras.insert("EXP_COMP1");
	regras.insert("EXP_COMP2");
	regras.insert("EXP_COMP3");
	regras.insert("EXP_COMP4");
	regras.insert("TIPO_FUNC");
	regras.insert("TIPO_VAR");
	regras.insert("LISTA_VARS1");
	regras.insert("LISTA_VARS2");
}

Node *criaRaiz(Token tk){
	Node *temp = new Node;
	Node *pai = new Node;
	temp->token = tk;
	temp->pai = pai;
	temp->pai->token.data = "RAIZ";
	return temp;
}

Node *newNode(Token tk, Node *pai){
	Node *temp = new Node;
	temp->pai = pai;
	temp->token = tk;
	return temp;
}

Node *newNodeSemPai(Token tk){
	Node *temp = new Node;
	temp->token = tk;
	return temp;
}

void imprimeArvore(Node *raiz){
	//if(raiz==NULL)
	//	return;

	queue <Node *> fila;
	fila.push(raiz);
	int nivel = 1;
	while(!fila.empty())
	{
		Node *tmp = fila.front();
		fila.pop();
		cout << "NIVEL: " << nivel << " PAI: " << endl;
		cout << "ATUAL: " << tmp->token.data << endl;
		cout << "FILHOS: ";
		for(int i=0;i<tmp->filhos.size();i++)
		{
			fila.push(tmp->filhos[i]);
			cout << tmp->filhos[i]->token.data << " ";
		}
		cout << endl << endl;
	}
}

void imprimeArvoreDFS(Node *raiz){
	
	if(raiz->token.id=="FUNC")
		declaracao_func(raiz);

	for(int i=0;i<raiz->filhos.size();i++)
		imprimeArvoreDFS(raiz->filhos[i]);

	//cout << "ID: " << raiz->token.id << " DATA: " << raiz->token.data << " ";

}

void declaracao_func(Node *raiz){
	for(int i=0;i<raiz->filhos.size();i++)
	{
		if(raiz->filhos[i]->token.data=="}")
		{
			raiz = raiz->filhos[i+1];
			return;
		}
		if(raiz->filhos[i]->token.data=="TIPO_FUNC")
		{
			if(lista_funcs.find(raiz->filhos[i+1]->token.data)!=lista_funcs.end())
				redecl_func=1;
			else
				lista_funcs.insert(raiz->filhos[i+1]->token.data);
		}
		if(raiz->filhos[i]->token.data=="LISTA_VARS1")
		{
			if(raiz->filhos[i]->filhos.size()>1)
				declaracao_vars_em_param(raiz->filhos[i]);
		}
		if(raiz->filhos[i]->token.data=="BLOCO_MAIN")
		{
			if(raiz->filhos[i]->filhos[0]->filhos[0]->token.data=="TIPO_VAR")
				declaracao_vars(raiz->filhos[i]->filhos[0]);

			executa_bloco(raiz->filhos[i]->filhos[0]);
		}
		if(raiz->filhos[i]->token.data=="INT_MAINHA")
		{
			if(raiz->filhos[i]->filhos[5]->filhos[0]->filhos[0]->token.data=="TIPO_VAR")
				declaracao_vars(raiz->filhos[i]->filhos[5]->filhos[0]);

			executa_bloco(raiz->filhos[i]->filhos[5]->filhos[0]);
		}
		

		/*if(raiz->filhos[i]->token.id=="id" && raiz->filhos[i+1]->token.id=="(" && raiz->filhos[i+2]->token.id=="FUNC_ARG1")
		{
			if(lista_funcs.find(raiz->filhos[i]->token.data)==lista_funcs.end())
				func_nao_declarada=1;
		}
		else if(raiz->filhos[i]->token.id=="id")
		{
			if(lista_vars.find(raiz->filhos[i]->token.data)==lista_vars.end())
				var_nao_declarada=1;
		}*/
	}

	lista_vars.clear();
}

void declaracao_vars_em_param(Node *raiz){
	int x;
	while(raiz->filhos.size()>1)
	{
		if(raiz->token.data=="LISTA_VARS1")
			x=1;
		else
			x=2;



		if(lista_vars.find(raiz->filhos[x]->token.data)!=lista_vars.end())
			redecl_var=1;
		if(lista_funcs.find(raiz->filhos[x]->token.data)!=lista_funcs.end())
			conflito_var_func=1;

		lista_vars.insert(raiz->filhos[x]->token.data);

		for(x=2;x<raiz->filhos.size();x++)
			if(raiz->filhos[x]->token.data=="LISTA_VARS2")
				break;

		raiz = raiz->filhos[x];
	}
}

void declaracao_vars(Node *raiz){
	int x;
	while(raiz->filhos[0]->token.data=="TIPO_VAR")
	{
		
		testa_atribuicao(raiz);

		if(lista_vars.find(raiz->filhos[1]->token.data)!=lista_vars.end())
			redecl_var=1;
		if(lista_funcs.find(raiz->filhos[1]->token.data)!=lista_funcs.end())
			conflito_var_func=1;

		lista_vars.insert(raiz->filhos[1]->token.data);

		for(x=2;x<raiz->filhos.size();x++)
			if(raiz->filhos[x]->token.data=="DECLARACAO")
				break;

		raiz = raiz->filhos[x];
	}
}

void executa_bloco(Node *raiz){
	int x;
	while(raiz->filhos[0]->token.data!="BLOCO")
	{
		for(x=0;x<raiz->filhos.size();x++)
			if(raiz->filhos[x]->token.data=="DECLARACAO")
				break;
		raiz=raiz->filhos[x];
	}

	percorreArvoreDFS(raiz);


}

void percorreArvoreDFS(Node *raiz){

	if(raiz->token.data=="EXP_ARITMETICA4")
	{
		if(raiz->filhos.size()==1 && raiz->filhos[0]->token.id=="id")
		{
			if(lista_vars.find(raiz->filhos[0]->token.data)==lista_vars.end())
				var_nao_declarada=1;
		}
		else if(raiz->filhos.size()>1 && raiz->filhos[0]->token.id=="id")
		{
			if(raiz->filhos[1]->token.data=="[")
			{
				if(lista_vars.find(raiz->filhos[0]->token.data)==lista_vars.end())
					var_nao_declarada=1;
			}
			else if(raiz->filhos[1]->token.data=="(")
			{
				if(lista_funcs.find(raiz->filhos[0]->token.data)==lista_funcs.end())
					func_nao_declarada=1;
			}
		}

	}
	else if(raiz->token.data=="FUNC_NADA")
	{
		if(lista_funcs.find(raiz->filhos[0]->token.data)==lista_funcs.end())
			func_nao_declarada=1;
	}
	else if(raiz->token.data=="LEITURA")
	{
		if(lista_vars.find(raiz->filhos[2]->token.data)==lista_vars.end())
			var_nao_declarada=1;
	}
	else if(raiz->token.data=="ATRIBUICAO")
	{
		if(lista_vars.find(raiz->filhos[0]->token.data)==lista_vars.end())
			var_nao_declarada=1;
	}


	for(int i=0;i<raiz->filhos.size();i++)
		percorreArvoreDFS(raiz->filhos[i]);


	//cout << "ID: " << raiz->token.id << " DATA: " << raiz->token.data << " ";

}

void testa_atribuicao(Node *raiz){

	if(raiz->token.data=="DECLARACAO" && porra==1)
	{
		porra=0;
		return;
	}
	else if(raiz->token.data=="DECLARACAO")
		porra=1;

	if(raiz->token.data=="EXP_ARITMETICA4")
	{
		if(raiz->filhos.size()==1 && raiz->filhos[0]->token.id=="id")
		{
			if(lista_vars.find(raiz->filhos[0]->token.data)==lista_vars.end())
				var_nao_declarada=1;
		}
		else if(raiz->filhos.size()>1 && raiz->filhos[0]->token.id=="id")
		{
			if(raiz->filhos[1]->token.data=="[")
			{
				if(lista_vars.find(raiz->filhos[0]->token.data)==lista_vars.end())
					var_nao_declarada=1;
			}
			else if(raiz->filhos[1]->token.data=="(")
			{
				if(lista_funcs.find(raiz->filhos[0]->token.data)==lista_funcs.end())
					func_nao_declarada=1;
			}
		}

	}

	for(int i=0;i<raiz->filhos.size();i++)
		testa_atribuicao(raiz->filhos[i]);
}