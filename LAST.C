#include <stdlib.h>
#include <stdio.h>

#include "string.h"
/*Описание переменных простых типов. Выражения. Операторы: условный, составной, цикла*/

#define TYPES      121   /* ТИП */
#define CONSTS     122   /* КОНСТАНТА */
#define VARS       123   /* ПЕРЕМЕННАЯ */
#define PROCS      124   /* ПРОЦЕДУРА */
#define FUNCS      125   /* ФУНКЦИЯ   */
/////////////////////code generation//////////////////////////////////////////////////////////////////////
typedef struct typerec TYPEREC;
int labelCounter=0;

#define REFERENCE 1 /*локальная переменная или параметр-значение*/
#define REGISTER  2 /* регистр */
#define CONSTANT  3 /* непосредственное данное */
#define REF_VAR   4 /* переменная, переданная по ссылке */

unsigned nocode=0;
FILE *output;

//////////////////////////////////////////////////////////////////////////////////////////////////////////
extern void Error(unsigned);

void block(unsigned *followers);
union const_val /* значение константы */
{
	int intval      /* целого  или символьного типа */ ;
	int boolval;
};

struct idparam
 { unsigned parhash; /* Значение Hash - функции */
   char *patidn; /* Ссылка на имя в таблице имен */
   TYPEREC *Type; /* Информация о типе параметра */
   int mettransf; /* Способ передачи */
   //struct prfun_as_par *par; /* Ссылка на информацию о параметрах параметра - процедуры */
   unsigned par_offset;/* смещение параметра относительно начала области данных */   
   struct idparam *linkparam; /* Ссылка на информацию о следующем параметре */
 };

struct treenode
{
	unsigned hashvalue   /* значение hash - функции */;
	char *idname        /* адрес имени в таблице имён */ ;
	unsigned clas      /* способ использования */;
    TYPEREC  *idtype; /* указатель на дескриптор типа */
	union
	{  /* для констант */
		union const_val constvalue; /* значение константы */
        /* для процедур ( функций )  */
        struct
        {
			struct idparam *param /* указатель  на информацию о параметрах */;
			int forw /* информация об опережающем описании */;
			int io; /*1-read, 2-readln, 3-write, 4-writeln*/
        } proc;
		struct
		{ 
			unsigned staticlevel;
			unsigned offset;
			unsigned param_var /* =TRUE,  если переменная является параметром, переданным по ссылке */;
		} vars;
	} casenode;

	struct treenode *leftlink ;
	struct treenode *rightlink;
}*CreatedNode = NULL, *LocalTree = NULL;
typedef struct treenode NODE;
//Структура дескриптора типа:
union variapart 
{ 
	TYPEREC  *basetype;
        /* для перечислимого типа */
        struct reestrconsts  *firstconst;  /* указатель на первый элемент списка констант */

};  /* end of union variant */

struct reestrconsts  /* структура элемента списка констант перечислительного типа*/
{
	char *addrconsts; /* адрес индентификатора в таблице имен */
	struct reestrconsts *next; /* указатель  на следующую структуру*/
};
struct typerec
{
	struct typerec *next;  /* указатель на следующий дескриптор типа */
	unsigned typecode;  /* код типа */
	union variapart casetype;
} *booltype, *inttype, *chartype, *stringtype, *vartype, *ConstantType = NULL;

//Это структура-описатель области действия localscope
struct scope
{
	struct treenode *firstlocal;
	TYPEREC *typechain;
	struct scope *enclosingscope;
	int count_locals /* размер области данных */;
	int level;
} *localscope;
typedef struct scope SCOPE;
int level=0;
//Открытие новой области действия
void open_scope()
{
	SCOPE *newscope;
	newscope = (SCOPE*)malloc(sizeof(SCOPE));
	newscope->firstlocal = NULL;
	newscope->typechain = NULL;
	newscope->enclosingscope = localscope;
	newscope->count_locals=0;
	newscope->level=level++;
	localscope = newscope;	
};
void dispose_types( TYPEREC* Item ) { /* Удаление Таблицы Типов */
  TYPEREC* PrevItem;
  while(Item!=NULL)
   { PrevItem=Item; 
     Item=Item->next; 
     free( (char*) PrevItem);
   };
};

void dispose_ids(struct treenode* Root) { /* Удаление Таблицы Идентификаторов */
   if(Root!=NULL)
   { dispose_ids(Root->leftlink); 
     dispose_ids(Root->rightlink); 
     free((char*) Root); 
   };
}
//Закрытие области действия
void close_scope()
{
	//localscope = localscope->enclosingscope;
	SCOPE *oldscope; 
	oldscope=localscope;
	localscope=localscope->enclosingscope;  /* Удаление Таблицы Идентификаторов */
	dispose_ids( oldscope->firstlocal);  /* Удаление Таблицы Типов */
	dispose_types( oldscope->typechain);  
	free( (char *) oldscope);
}

//Внесение нового дескриптора типа в таблицу типов
TYPEREC *newtype(int tcode)
{
	struct typerec *new1;
	new1 = (struct typerec*)malloc(sizeof(struct typerec));
	new1->typecode = tcode;
	new1->next = localscope->typechain;
	//Вставляем вершину в список. В зависимости от типа настраиваем начальные значения
	
	switch (new1->typecode)
	{
		//case LIMITEDS:
		//	new1->casetype.basetype = NULL;
		case SCALARS:
			break;
		case ENUMS:
			new1->casetype.firstconst = NULL;
			break;
	};
	localscope->typechain = new1;
	return(new1);
}

//Внесение нового идентификатора в таблицу идентификаторов
//В переменную CreatedNode вносится ссылка на созданный элемент
NODE *newident(NODE *Tree,unsigned hashfunc,char *addrname,int classused)
{
	NODE *tree;
	int flag = 0;
	tree = Tree;
	//Проводим поиск текущего идентификатора в данной области действия.
	//За его присутствие отвечает переменная flag
	while ((tree != NULL) && (flag != 1))
	{
		//Если нашли - flag стави на 1
		if ((tree->idname == addrname) && (tree->clas == classused))
		{
			if ((tree->clas==FUNCS || tree->clas==PROCS) && tree->casenode.proc.forw)
			{
				CreatedNode=tree;
				return Tree;
			}
			else
				flag = 1;
		}
		//Если нет - ищем дальше по дереву. В зависимости от хеш-функции.
		else
			if (tree->hashvalue  < hashfunc)
			{
				tree=tree->rightlink ;
			}
			else
			{
				tree=tree->leftlink ;
			}
	}
	//Если переменная существует, то выводим ошибку
	//и возвращаем ссылку на этот идентификатор
	if (flag == 1)
	{
		
		Error(101); 
		nocode=1;
		//return Tree;
	}
	//Если не найден
	else
	{
		//Пришли в конец - вставляем
		if (!Tree) 
		{
			Tree = (NODE *) malloc(sizeof(NODE));
			Tree->hashvalue = hashfunc;
			Tree->clas = classused;
			Tree->idname = addrname;
			Tree->casenode.vars.offset=localscope->count_locals+INT_LENGTH;
			localscope->count_locals+=INT_LENGTH;
			Tree->leftlink = NULL;
			Tree->rightlink = NULL;
			CreatedNode = Tree;
		}
		//Не нашли - идем дальше рекурсивно
		else
			if (hashfunc < Tree->hashvalue) // Либо влево либо вправо
			{
				Tree->leftlink =newident(Tree->leftlink,hashfunc,addrname,classused);
			}
			else
			{
				Tree->rightlink =newident(Tree->rightlink ,hashfunc,addrname,classused);
			}
		//return Tree; //Возвращаем это дерево
	}
	return Tree; 
};

typedef struct reestrconsts LIST;

//Добавление новой константы в дескриптор перечисляемого типа.
//Тупо вставка в начало
LIST* newcons(LIST *List, char *addrname)
{
	LIST *nov;
	nov = (LIST *) malloc(sizeof(LIST));
	nov->addrconsts = addrname;
	nov->next = List;
	return nov;
}

//Элемент вспомогательного списка
struct listrec
{
	struct treenode *id_r;
	struct listrec *next;
}*varlist;

//Поиск идентификатора в таблице. Сам же и выдает ошибку если его там нет
//А если есть, то возвращает ссылку на него
NODE *SearchIdent (SCOPE* local,char *addrname,unsigned hashfunc)
{
	int flag=0;
	SCOPE* current = local;
	NODE* Tree = local->firstlocal;
	//Поиск по дереву элемента во всех областях действия
	while (flag == 0)
	{
		Tree = current->firstlocal;
		while ((Tree != NULL) && (flag != 1)) 
		{
			if ((Tree->idname == addrname))
			{
				flag = 1; //Нашли
			}
			else
				if (Tree->hashvalue  < hashfunc)
				{
					Tree=Tree->rightlink ;
				}
				else
				{
					Tree=Tree->leftlink ;
				};
		}
		//переход на другую область действия если не нашли в этой
		if (current->enclosingscope != NULL) 
			current = current->enclosingscope;
		else break;
	}
	//Если элемент не найден - выводим ошибку о том, что имя не описано
	if (flag == 0)
	{
		
		return NULL;
	}
	//else
		//Возвращаем ссылку на этот идентификатор
		return Tree;
}
/*void bssSection(NODE* tree)
 {
	 //char* str=(char*)malloc( sizeof(*byte2) * 100 );
		if (tree != NULL)
		{
				if (tree->clas==VARS)
				{					

					if (tree->idtype == booltype || tree->idtype == inttype || tree->idtype == stringtype)
						fprintf(output,"%s: resd 1\n",tree->idname);
				}
			
			else
			{
					bssSection(tree->rightlink) ;
					bssSection(tree->leftlink) ;				
			}
		}	 
 }*/
//Заносит новую переменную в список переменных, ожидающих указанмя типа
//(когда в vardeclaration описали все переменные, а тип - тока в конце написан
//тогда все они заносятся в очередь и в конце получают тип
void newvariable()
{
	//Опять просто вставка в начало списка
	struct listrec *listentry;
	if (symbol == ident)
	{
		listentry = (struct listrec*)malloc(sizeof(struct listrec));
		localscope->firstlocal = newident(localscope->firstlocal,hashresult,addrname,VARS);
		listentry->id_r = CreatedNode;
		listentry->next = varlist;
		varlist = listentry;

	};
};

//Это как раз приписывание типа всем переменным, ожидающим в очереди
void addattributes()
{
	 struct listrec *listentry, *oldentry;
	 listentry = varlist;
	 //Пройдем по порядочку
	 while (listentry!=NULL)
	 {
		 listentry->id_r->idtype = vartype;
		if (localscope->level==1)
		{
		 if (!nocode)
			//bssSection(listentry->id_r);
			fprintf(output,"%s: resd 1\n",listentry->id_r->idname);
		 //else fclose(output);
		}
		 oldentry = listentry;
		 listentry = listentry->next;
		 //Очищаем элемент очереди после того, как дописали
		 free((void *) oldentry);
	 }
}

//Проверка совместимости типов
 int Compatible(TYPEREC* f, TYPEREC* s)
 {
	if ((s == NULL) || (f == NULL)) return 0; //Это если один из типов не правильный, то сразу выйдем, чтобы без ошибок
	if (f == s) 
		return 1;
	return 0;
 }

////////////////
//Главная часть
////////////////
 
//Объявления 
 TYPEREC* simpletype();
 TYPEREC* arraytype();
 TYPEREC* expression ();
 void operatore();
 TYPEREC* SimpleExpression();
//Фунцкии

 //Функция IsConstant возвращает 1, если символ - числовая константа.
 //Иначе возвращает 0. В переменную COnstantType заносится ссылка на дескриптор типа константы.
 int IsConstant(unsigned smb)
 {
    if (smb == stringc)
	{
		ConstantType = stringtype; //Возвращаем дескриптор типа real
		return 1;
	}
	if (smb == intc)
	{
		ConstantType = inttype;	//Возвращаем дескриптор типа int
		return 1;
	}
	if (smb == charc)
	{
		ConstantType = chartype;	//Возвращаем дескриптор типа char
		return 1;
	}
	return 0;
 }
 //Функция "тип" - проверяет синтаксис и семантику описания типа, возвращая ссылку на 
 //дескриптор этого типа
 TYPEREC* type (unsigned *followers)
{
	//Это дескриптор типа, который мы будем возвращать
	TYPEREC* TypeEntry = NULL;

	if(!belong (symbol, st_typ))
	{
		Error(18);
		skipto2(st_typ, followers);
		nocode=1;
	}
	//Иначе ссылку на дескриптор простого типа
	else TypeEntry = simpletype (followers);

	if (!belong (symbol, followers))
	{
		Error(6);
		skipto1(followers);
		nocode=1;
	}
	//Возврат значения
	return TypeEntry;
}

 //Функция Simpletype. Анализ простого типа. Возвращает ссылку на его дескриптор
 TYPEREC* simpletype (unsigned *followers)
 {
	 NODE* Ident = NULL;//, *Ident2 = NULL;
	 TYPEREC* TypeEntry = NULL;
	 //Нейтрализация синтаксических ошибок
	 if (!belong (symbol, st_simpletype))
	 {
		 Error (18);
		 skipto2(st_simpletype, followers);
		 nocode=1;
	 }
	 //Синтаксический и семантический анализ
	 if (symbol == ident) 
	 //Либо это уже известный тип, либо ограниченный
	 //Пример: integer - ident. Или a..b - ограниченный. Тоже начинается с ident.
	 {
		 //Находим этот идентификатор в нашей таблице
		 Ident = SearchIdent(localscope, addrname, hashresult);
		 if (Ident==NULL) {Error(104);nocode=1;}
		 nextsym ();
			 if ((Ident != NULL) && (Ident->clas != TYPES))
			 {
				 Error(100);
				 nocode=1;
			 }
		 //Возвращаем значение
		 if ((Ident != NULL)) TypeEntry = Ident->idtype;
		 else TypeEntry = NULL;

	 }
	 
	 //Нейтрализация синтаксических ошибок
	 if (!belong (symbol, followers))
	 {
		 Error (6);
		 skipto1(followers);
		 nocode=1;
	 }
	 return TypeEntry;
 }

 void vardeclaration (unsigned *followers)
{
	if (!belong (symbol, idstarters))
	{
		Error (18);
		skipto2(idstarters, followers);
		nocode=1;
	}
	//Если нашли описание переменных
	if (symbol == ident)
	{
		varlist = NULL; //Инициализируем очередь переменных, ожидающих тип
		newvariable();  //Вносим туда текущую переменную
		nextsym();
		while (symbol == comma)
		{
			nextsym();
			newvariable(); //Вносим переменные в список ожидания
			accept(ident);
		}
		accept(colon);
		vartype = type(followers); //Узнаем тип всех этих переменных
		addattributes(); //Присваиваем всем им этот тип

		if (!belong (symbol, followers))
		{
			Error(6);
			skipto1(followers);
			nocode=1;
		}
	}
}

 //Обработка описания массива
 //TYPEREC* arraytype(unsigned *followers)
 //{
	//TYPEREC* TypeEntry = newtype(ARRAYS); //Создаем новый дескриптор типа массива
	//struct indextyp* ptr = NULL;

	//accept (arraysy); 
	//accept (lbracket); 

	////Инициализируем его вариантную часть. Выделяем память
	//TypeEntry->casetype.arraytype.indextype = (struct indextyp*)malloc(sizeof(struct indextyp*));
	////Заносим тип первого индекса в дескриптор типа массива
	//TypeEntry->casetype.arraytype.indextype->Type = simpletype (af_simpletype);

	////Ставим указатель на первый индекс массива
	//ptr = TypeEntry->casetype.arraytype.indextype;
	////NEXT делаем NULL чтобы избежать ошибок
	//ptr->next = NULL;

	////Пока запятая, считываем индексы по-очереди
	//while (symbol == comma)
	//{
	//	nextsym();
	//	//Выделяем память под следующий индекс
	//	ptr->next = (struct indextyp*)malloc(sizeof(struct indextyp*));
	//	//Записываем этот тип в дескриптор типа массива
 //       ptr->next->Type = simpletype (af_simpletype);
	//	//Переводим указатель и следующий делаем NULL
	//	ptr = ptr->next;
	//	ptr->next = NULL;
	//}

	//accept (rbracket); 
	//accept (ofsy);
	//
	////Тип элементов, из которых состоит массив считываем, как полагается, в конце
	//TypeEntry->casetype.arraytype.basetype = type(followers);
	//return TypeEntry;
 //}
 
//Описание раздела переменных. Тут только синтаксис возможен.
 void varpart (unsigned *followers)
 {
	 unsigned ptra [SET_SIZE];
	 if (!belong (symbol, st_varpart))
	 {
		 Error (18);
		 skipto2(st_varpart, followers);
		 nocode=1;
	 }
	 if (symbol == varsy)
	 {
		 
		 accept (varsy);
		 SetDisjunct(af_1constant, followers, ptra);
		 vardeclaration (ptra);
		 accept (semicolon);
		 while (symbol == ident)
		 {
			 vardeclaration (ptra);
			 accept (semicolon);
		 }
		 if (!belong (symbol, followers))
		 {
			 Error (6);
			 skipto1(followers);
			 nocode=1;
		 }
	 }

	//else
	//	fclose(output);
	
 }
int varval;
struct idparam *funclistend;
void newparam() 
{
	//пополняет список параметров
	struct idparam	*par;
	if (symbol==ident){
		par=(struct idparam*) malloc (sizeof (struct idparam));
		if (funclistend!=NULL) 
		{
			par->patidn=addrname;
			par->linkparam=NULL;
			par->mettransf=varval;
			par->Type=NULL;
			funclistend->linkparam=par;
			funclistend=par;
		}
		else 
		{
			par->patidn=addrname;
			par->linkparam=NULL;
			par->mettransf=varval;
			par->Type=NULL;
			LocalTree->casenode.proc.param=par;
			funclistend=par;
		}
	}
}

void addpartyp() { //использует funclist и заносит тип в ТПараметров
	struct idparam	*par;
	par=LocalTree->casenode.proc.param;//funclistend;//CreatedNode->casenode.proc.param;
	while (par->Type!=NULL) 
		par=par->linkparam;
	while (par!=funclistend->linkparam){
		par->Type=vartype;
		par=par->linkparam;
	}
}

void groupofparam(unsigned *followers)
{
	unsigned ptra[SET_SIZE];
	if(!belong(symbol,idstarters))
	{
		Error(2);
		skipto2(idstarters,followers);
		nocode=1;
	}
	if(belong(symbol,idstarters))
	{
		varlist = NULL;//начало занесения в список параметров
		newvariable();
		newparam();
		accept(ident);
		while(symbol==comma)
		{
			nextsym();//
			newvariable();
			newparam();
			accept(ident);
		}
		accept(colon);
		SetDisjunct(af_sameparam,followers,ptra);//rightpar,semicolon,eolint
		vartype = type(ptra);
		addattributes();
		addpartyp();
		if(!belong(symbol,followers)){
			Error(7);
			skipto1(followers);
			nocode=1;
		}
	}
}

void formalparametr(unsigned *followers)
{
	unsigned ptra[SET_SIZE];
	SetDisjunct(st_varpart,idstarters,ptra);
	if(!belong(symbol,ptra))
	{
		Error(7);
		skipto2(ptra,followers);
		nocode=1;
	}
	if(!belong(symbol,st_procfuncpart))
	{
		SetDisjunct(af_sameparam,followers,ptra);
		switch(symbol)
		{
		case (varsy):accept(varsy);
			varval = 1;
			groupofparam(ptra);
			break;
		case (ident):
			varval = 2;
			groupofparam(ptra);
			break;
		}
		if(!belong(symbol,ptra))
		{
			Error(6);
			skipto1(ptra);
			nocode=1;
		}
	}
}

void procfuncpart(unsigned *followers)
{
	unsigned ptra[SET_SIZE];
	NODE *Ident = NULL;
	TYPEREC *typevar;
	struct idparam	*par;
	if(!belong(symbol,st_procfuncpart))
	{
		Error(18);//6
		skipto2(st_procfuncpart,followers);
		nocode=1;
	}
	 if(belong(symbol,st_procfuncpart))
	 {
		 while (symbol==proceduresy||symbol==functionsy)
		 {

			SetDisjunct(st_varpart,followers,ptra);
			switch(symbol)
			{
			case functionsy:
				accept(functionsy);
				{
					localscope->firstlocal=newident(localscope->firstlocal, hashresult,addrname,FUNCS);
					if (!nocode)
					{
						fprintf(output, "\n%s:\n",addrname);
						fprintf(output,"push ebp\n");
						fprintf(output,"mov ebp, esp\n");
					}				
					LocalTree=CreatedNode;
					if (LocalTree->casenode.proc.forw!=1)
					{
						CreatedNode->casenode.proc.param=NULL;
						CreatedNode->idtype=NULL;//????
						open_scope();
						accept(ident);
						funclistend=NULL;
						SetDisjunct(af_funclistparam,followers,ptra);
						if (symbol==leftpar)
						{
							nextsym();
							formalparametr(followers);
							while (symbol==semicolon)
							{
								nextsym();
								formalparametr(followers);
							}
							accept(rightpar);
						}
						accept(colon);
						//accept(ident);
						SetDisjunct(af_1constant,followers,ptra);//semicolon, eolint
						LocalTree->idtype=simpletype(ptra);//CreatedNode
					}
					else
					{
						open_scope();
						accept(ident);
						par=LocalTree->casenode.proc.param;
						while (par!=NULL)
						{
							SearchInTable(par->patidn);
							localscope->firstlocal = newident(localscope->firstlocal,hashresult,addrname,VARS);
							CreatedNode->idtype=par->Type;
							par=par->linkparam;
						}
						accept(colon);
						SetDisjunct(af_1constant,followers,ptra);//semicolon, eolint
						typevar=simpletype(ptra);//CreatedNode					
						if (!Compatible(LocalTree->idtype, typevar))
						{
							Error(189);	
							nocode=1;
						}
					}
					//else
				//	LocalTree->casenode.proc.param=funclistend;//CreatedNode
					//ent->idtype=entry1->idtype;		
					accept(semicolon);
				}
/*				if(!belong(symbol,followers))
				{
					Error(123);
					skipto1(followers);
				}*/		
			break;
			case proceduresy:
				accept(proceduresy);
				localscope->firstlocal=newident(localscope->firstlocal, hashresult,addrname,PROCS);
				if (!nocode)
				{
					fprintf(output, "%s:\n",addrname);
					fprintf(output,"push ebp\n");
					fprintf(output,"mov ebp, esp\n");
				}
				//else
				//	fclose(output);				
				LocalTree=CreatedNode;
				if (LocalTree->casenode.proc.forw!=1)
				{
					CreatedNode->casenode.proc.param=NULL;
					CreatedNode->idtype=NULL;
					open_scope();
					accept(ident);
					SetDisjunct(af_proclistparam,followers,ptra);
					//if(symbol==leftpar)parameters_list(ptra);
					if (symbol==leftpar)
					{
						nextsym();
						formalparametr(followers);
						while (symbol==semicolon)
						{
							nextsym();
							formalparametr(followers);
						}
						accept(rightpar);
					}
					LocalTree->idtype=simpletype(ptra);
				}
				else
				{
					open_scope();
					accept(ident);		

					par=LocalTree->casenode.proc.param;
					while (par!=NULL)
					{
						SearchInTable(par->patidn);
						localscope->firstlocal = newident(localscope->firstlocal,hashresult,addrname,VARS);
						CreatedNode->idtype=par->Type;
						par=par->linkparam;
					}
				}
				accept(semicolon);
				break;
			}
			//common part for proc & func

			switch (symbol)
			{

				case forwardsy:
					nextsym();
					LocalTree->casenode.proc.forw=1;
					accept(semicolon);
					break;
				default:
					if (LocalTree->casenode.proc.forw!=1)
					LocalTree->casenode.proc.forw=0;
				if (belong(symbol,st_varpart))
				{
					SetDisjunct(af_blockprocfunc,followers,ptra);
					block(ptra);
					if (!nocode)
					{
						fprintf(output,"sub esp, %d\n",  localscope->count_locals + 4);
						fprintf(output,"mov esp, ebp\n");
						fprintf(output,"pop ebp\n");
						fprintf(output,"ret\n");
					}
					accept(semicolon);
				}
				break;
			}
			close_scope();
			if(!belong(symbol,followers))
			{
					Error(1);//6
					skipto1(followers);
					nocode=1;
			}
		 }
	 }
}

//Блок. Только синтаксис.
 void block (unsigned *followers)
 {
	unsigned ptra[SET_SIZE];
	if (!nocode)
	{
		if (localscope->level==1)
		{
			fprintf(output,"section .data\n");
			fprintf(output,"int_format dd \"%%d\", 10, 0\n");
			fprintf(output,"str_format dd \"%%s\", 10, 0\n");
			fprintf(output,"section .bss\n");
		}
	}
	SetDisjunct(followers, begpart, ptra);
	varpart (begpart); 
	if (!nocode)
	{
		if (localscope->level==1)
		{
			fprintf(output,"section .text\n");
			fprintf(output,"global main\n");
		}
	}
	procfuncpart(ptra);
	////////////
	if (!nocode)
	{
		if (localscope->level==1)
		{
			fprintf(output,"main:\n");
			fprintf(output,"push ebp\n");
			fprintf(output,"mov ebp, esp\n");
			fprintf(output,"sub esp, %d\n", localscope->count_locals + 4);
		}
	}
	
	operatore(followers);
	
 }
 void StandartProc(unsigned *followers, int io)
 {	 
	unsigned ptra[SET_SIZE];
	struct idparam *ptrparam;
	NODE *Ident = NULL;
	//NODE *Ident;
	TYPEREC *typevar, *h_ptr;
	nextsym();
	switch (io)
	{
		case 1://read
		case 2://readln
			if (symbol == leftpar)
			{
			//	accept(leftpar);
				 do
				 {
					nextsym();
				
					Ident = SearchIdent(localscope, addrname, hashresult);	
					if (Ident==NULL && !(symbol==intc || symbol==charc || symbol==stringc)) 
					{
						Error(104);
						nocode=1;
					}
					if (symbol==intc || symbol==charc || symbol==stringc)
					{
						Error(6);
						nocode=1;
					}
					//else
					nextsym();
					if(symbol!=comma)
						if(symbol!=rightpar)
						{
							Error(6);
							skipto1(af_factparam);
							nocode=1;
						}
						if (!nocode)
						{
							fprintf(output,"pusha\n");
							fprintf(output,"lea ebx, [ebp - %d]\n", Ident->casenode.vars.offset);
							fprintf(output,"push ebx\n");
							fprintf(output,"push int_format\n");
							fprintf(output,"call scanf\n");
							fprintf(output,"add esp, 8\n");
							fprintf(output,"popa\n");
						}
				 } while (symbol == comma);
		//		 accept(rightpar);
			}
			//accept(semicolon);
			break;
		case 3://write
		case 4://writeln
			if (symbol == leftpar)
			{
				//accept(leftpar);
				do
				{
					nextsym();

					typevar = expression(followers);	
					if (typevar == NULL)
						nextsym();
					if(symbol!=comma)
						if(symbol!=rightpar)
						{
							Error(6);
							skipto1(af_factparam);
							nocode=1;
						}
						if (!nocode)
						{
							fprintf(output,"push int_format\n");
							fprintf(output,"call printf\n");
							fprintf(output,"add esp, 8\n");
						}
				} while (symbol == comma);
			//	accept(rightpar);
			}
	//		accept(semicolon);
			break;
	}
 }
 void CallProcFunc(unsigned *followers, struct treenode *ptr_proc)
 {
	 unsigned ptra[SET_SIZE];
	 struct idparam *ptrparam;
	 TYPEREC *typevar, *h_ptr;
	 NODE* Ident = NULL;
	 nextsym();
	 ///////////
	// SetDisjunct(af_factparam, followers, ptra);
	 if (symbol == leftpar)
	 {
		 ptrparam = ptr_proc->casenode.proc.param;
		 if (ptrparam == NULL)
		 {
			 Error(126);
			 nocode=1;
		 }
		 else
		 {
			 do
			 {
				 nextsym();
				 h_ptr = ptrparam->Type;
				 switch (ptrparam->mettransf)
				 {
					 case 1:
						// typevar = variable(ptra);
						 
						 Ident = SearchIdent(localscope, addrname, hashresult);
						 if (Ident==NULL) {Error(104);nocode=1;}
						 else
						{
						 typevar = (Ident->idtype);
						 if (!Compatible(h_ptr, typevar))
						 {
							 Error(189);
							 nocode=1;
						 }
						}
						 accept(ident);

						// gen_param_var(ptrparam);
						 break;
					 case 2:
						typevar = expression(followers);
						if (typevar == NULL) 
						{
							Error(126);
							nocode=1;
						}
						else
							if (!Compatible(h_ptr, typevar))
							{
								Error(189);
								nocode=1;
							}
						 break;
				 }
				 ptrparam = ptrparam->linkparam;
				if(symbol!=comma)
					if(symbol!=rightpar)
					{
						Error(6);
						skipto1(af_factparam);
						nocode=1;
					}
		//		 accept(ident);
			 } while (symbol == comma && ptrparam!=NULL);
			 if (ptrparam!=NULL) //если число формальных и факт парам-в не равно
			 {
				 Error(126);
				 nocode=1;
			 }
			 
		 }
	 }
 }
 //Оператор.
 void operatore (unsigned *followers)
 {
	 int localLabelCounter = labelCounter++;
	 TYPEREC* exptyp;	//Тип выражения
	 NODE* cond = NULL /*Идентификатор-условие*/, *caseid = NULL /**/; 
	 struct textposition currentpos, exprpos; //Позиция в тексте (чтобы вывести ошибку в нужном месте)
	 unsigned ptra [SET_SIZE];

	 if (!belong (symbol, st_statement))
	 {
		 Error(6);
		 skipto2(st_statement, followers);
		 nocode=1;
	 }
	 //Составной оператор. Семантики у него нет
	 if (symbol == beginsy)
	 {
		 SetDisjunct (af_compstatement, followers, ptra);
		 accept (beginsy);
		 operatore (ptra);
		 while (symbol == semicolon)
		 {
			 accept(semicolon);
			 operatore (ptra);
		 }
		 accept (endsy);
	 }
	 //Условный оператор
	 else if (symbol == ifsy)
	 {

		 SetDisjunct (af_iftrue, followers, ptra);
		 accept (ifsy);

		 exprpos = token; //Запоминаем позицию в тексте
		 exptyp = expression (ptra); //Анализируем выраженеи
		 currentpos = token; //Записываем позицию после выражения
		 if ((exptyp == NULL) || (!Compatible(exptyp, booltype))) //Если тип не совместим с булевым
		 {
			 token = exprpos; //Вернемся назад
			 Error(145);	//Выведем ошибку
			 token = currentpos; //И обратно вперед
			 nocode=1;
		 }

		 if (!belong(symbol, ptra))
		 {
			Error(6);
			skipto1(ptra);
			nocode=1;
		 }	 
		 SetDisjunct (af_iffalse, followers, ptra);
		 accept (thensy);
		 if (!nocode) 
		 {

			fprintf(output,"pop eax\n");
			fprintf(output,"cmp eax, 1\n");
            localLabelCounter++;
            fprintf(output,"jne L%d\n", localLabelCounter);
		 }
		 operatore (ptra);
		 if (symbol == elsesy)
		 {
			 if (!nocode) 
			 {
				 fprintf(output,"jmp L%d\n", (localLabelCounter + 1));
				 fprintf(output,"L%d:\n", localLabelCounter);
				 localLabelCounter++;
			 }
			 accept (elsesy);
			 operatore (followers);
		 }
		 fprintf(output,"L%d:\n", localLabelCounter);
		 labelCounter=localLabelCounter;
	 }

	 else if (symbol==whilesy)
	 {
		nextsym ();
		SetDisjunct(af_whilefor, followers, ptra);
		exptyp = expression(ptra);
		if (exptyp != booltype) {Error (135);nocode=1;}
		accept(dosy);
		if (!nocode)
		{
            fprintf(output,"pop eax\n");
            fprintf(output,"cmp eax, 1\n");
            localLabelCounter++;
            fprintf(output,"jne L%d\n",  localLabelCounter);
            fprintf(output,"L%d:\n",(localLabelCounter + 1));
		}
		operatore(followers);
		if (!nocode)
		{
            fprintf(output,"pop eax\n");
            fprintf(output,"cmp eax, 1\n");
            fprintf(output,"je L%d\n", (localLabelCounter + 1));
            fprintf(output,"L%d:\n", localLabelCounter);
            labelCounter = localLabelCounter;
		}
	 }
	 else if (symbol==repeatsy)
	 {
		SetDisjunct(af_repeat, followers, ptra);
		if (!nocode)
		{
            fprintf(output,"L%d:\n",(localLabelCounter));
		}
		operatore(ptra);
		while (symbol == semicolon)	
		{
			nextsym();
			operatore(ptra);
		}
		accept(untilsy);
		exptyp = expression(followers);
		if (!nocode)
		{
            fprintf(output,"pop eax\n");
            fprintf(output,"cmp eax, 1\n");
            fprintf(output,"je L%d\n", (localLabelCounter));
            labelCounter = localLabelCounter;
		}
		if (exptyp != booltype) {Error (135);nocode=1;}

	 }
	 //Присваивание
	 else if (symbol == ident)
	 {
		 NODE* Ident = NULL;
		 Ident = SearchIdent(localscope, addrname, hashresult);
		 if (Ident==NULL) {Error(100);nocode=1;}//4);
		 else
		 {
			 if (Ident->clas == VARS || Ident->clas == FUNCS)
			 {

		//		 if (Ident->clas == VARS)
			//		 SetDisjunct(af_assignment,followers,ptra);
				 accept(ident);
				 accept(assign);
		//		 else
		//			 SetDisjunct(af_factparam,followers,ptra);
				 exptyp=expression(followers);
				 if (!Compatible(Ident->idtype,exptyp))
				 {
					 Error(145);
					 nocode=1;
				 }
				 if (!nocode)
				 {
					 fprintf(output,"pop eax\n");
					 fprintf(output,"mov %s, eax\n",Ident->idname);
				 }
			 }
			 else 
				 if (Ident->clas == PROCS)
				 {
					 SetDisjunct(af_factparam,followers,ptra);
					 if (Ident->casenode.proc.io > 0)
						StandartProc(followers,Ident->casenode.proc.io);
					 else
						CallProcFunc(followers, Ident);		
					 accept(rightpar);
				 }
					 //Error(100);
		 }		 
	 }
	if (!belong (symbol, followers))
	{
		Error(6);
		nocode=1;
		skipto1(followers);
	}
 }

 
//Проверки арифметических операция - тупо перебором

 //Проверка выражения на принадлежность логическому типу
TYPEREC* logical(TYPEREC* exptype)
{
	if (exptype == NULL) return NULL;
	if ((exptype == booltype))
		return booltype;
	Error(145);
	nocode=1;
	return NULL;
}

//Проверка корректности умножения
TYPEREC* test_mult(TYPEREC* exptype1, TYPEREC* exptype2)
{
	if ((exptype1 == NULL) || (exptype2 == NULL)) return NULL;
	if ((exptype1->typecode == SCALARS) && (exptype2->typecode == SCALARS) && (exptype1 != booltype)
		&& (exptype2 != booltype))
			return exptype1;
	Error(145);
	nocode=1;
	return NULL;
}

//Проверка корректности сложения
TYPEREC* test_add(TYPEREC* exptype1, TYPEREC* exptype2)
{
	if ((exptype1 == NULL) || (exptype2 == NULL)) return 0;
	if ((exptype1->typecode == SCALARS) && (exptype2->typecode == SCALARS) && (exptype1 != booltype)
		&& (exptype2 != booltype)) 
			return exptype1;
	Error(145);
	nocode=1;
	return NULL;
}

//Проверка корректности сравнения
TYPEREC* test_comparing(TYPEREC* exptype1, TYPEREC* exptype2, unsigned operation)
{
	if ((exptype1 == NULL) || (exptype2 == NULL)) return 0;
	if ((exptype1->typecode == SCALARS) && (exptype2->typecode == SCALARS) && ((operation == later) || (operation == greater) || (operation == laterequal)|| (operation == greaterequal) || (operation == equal) || (operation == latergreater)))
		return booltype;
	if ((exptype1->typecode == SCALARS) && (exptype2->typecode == SCALARS) && (operation != andsy) && (operation != orsy) && (operation != notsy)) 
			return exptype1;
	if ((exptype1 == booltype) && (exptype2 == booltype) && ((operation == andsy) || (operation == orsy) || (operation == notsy) || (operation == equal) || (operation == latergreater))) 
			return exptype1;
	Error(145);
	nocode=1;
	return NULL;
}

//Проверка правильности знака перед выражением
int right_sign(TYPEREC* exptype)
{
	if (exptype == NULL) return -1;
	return ((exptype == inttype));
}

//Выражение
 TYPEREC* expression (unsigned *followers)
 {

	 TYPEREC* ex1type = NULL, *ex2type; //Типы операндов
	 unsigned operation; //Код операции
	 int localLabelCounter=labelCounter++;
	 if (!belong (symbol, st_express))
	 {
		 Error(6);
		 skipto2(st_express, followers);
		 nocode=1;
	 }
	 ex1type = SimpleExpression (followers); //Тип первого выражения
	 //Если символ - символ операции сравнения
	 if (belong(symbol, op_rel))
	 {
		 operation = symbol; //Запомним операцию
		 nextsym();			
		 ex2type = SimpleExpression (followers);	//Тип второго выражения
		 ex1type = test_comparing(ex1type, ex2type,operation); //Проверка совместимости
	 
	 if (!nocode)
	 {
		localLabelCounter++;
		fprintf(output,"pop ebx\n");
		fprintf(output,"pop edx\n");

		fprintf(output,"cmp edx, ebx\n");
		switch (operation)
		{
		case latergreater:
			fprintf(output,"jnz L%d\n",localLabelCounter);
			break;
		case equal:
			fprintf(output,"jz L%d\n",localLabelCounter);
			break;
		case laterequal:
			fprintf(output,"jbe L%d\n",localLabelCounter);
			break;
		case greaterequal:
			fprintf(output,"jae L%d\n",localLabelCounter);
		case later:
			fprintf(output,"jb L%d\n",localLabelCounter);
			break;
		case greater:
			fprintf(output,"ja L%d\n",localLabelCounter);
			break;
		}		
		fprintf(output,"mov eax, 0\n");
		fprintf(output,"jmp L%d\n",(localLabelCounter + 1));

		fprintf(output,"L%d:\n",(localLabelCounter));
		fprintf(output,"mov eax, 1\n");

		fprintf(output,"L%d:\n",(localLabelCounter + 1));
		fprintf(output,"push eax\n");
	 }	 
	 labelCounter=localLabelCounter;
 }
	 return ex1type;
 }

 //Множитель
 TYPEREC* factor (unsigned *followers)
 {
	unsigned ptra [SET_SIZE];
	TYPEREC* exptype = NULL;
	NODE* node; //Указатель на вершину ТИ для текущего идентификатора
	 if (!belong (symbol, st_termfact))
	 {
		 Error(6);
		 skipto2(st_termfact, followers);
		 nocode=1;
	 }
	 //Либо это константа
	 if (IsConstant(symbol))
	 {
		 exptype = ConstantType;
		 
		 if (!nocode)
		 {
			 switch(symbol)
			 {
				case stringc:
					fprintf(output,"push dword %s\n", strings);
					break;

				case intc:	
					fprintf(output,"push dword %d\n", nmbi);
					break;
	
				case charc:	
					fprintf(output,"push dword %c\n", onesymbol);
					break;
	
			 }
		 }
		 nextsym ();
	 }
	 //Либо идентификатор
	 else if (symbol == ident)
	 {
		 //Находим его
		node = SearchIdent(localscope, addrname, hashresult);
		if (node==NULL) { Error(104); exptype = NULL; nocode=1;}
		//Узнаем тип
		else
			switch(node->clas)
		{
			case VARS:
				exptype = node->idtype;//variable(followers); ??????
				nextsym();
				if (!nocode)
					fprintf(output,"push dword %s\n", node->idname);
				break;
			case CONSTS:
				exptype = node->idtype;
				nextsym();
				if (!nocode)
				{
					fprintf(output,"push dword %d\n", node->casenode.constvalue.boolval);
					fprintf(output,"push eax\n");
				}				
				break;
			case FUNCS:
				exptype = node->idtype;
				SetDisjunct(af_factparam,followers,ptra);
				CallProcFunc(followers, node);
				accept(rightpar);
				if (!nocode)
				{
					fprintf(output,"call %s\n", node->idname);
					fprintf(output,"push eax\n");
				}
				break;
			default:
				exptype = NULL;
				nextsym();
		}
		//else
		//	exptype = NULL;//StandartFunc(followers);//NULL;
		// !!!!!!!!!!!!!!!! закомменнчено, но не факт,что верно 
	 }
	 //Скобка
	 else if (symbol == leftpar)
	 {
		 nextsym();
		 //Анализируем выражение в скобках
		 exptype = expression (followers);
		 accept (rightpar);
	 }
	 //Если NOT 
	 else if (symbol == notsy)
	 {
		 accept (notsy);
		 //Анализируем выражение и проверку его принадлежности к логическим
		 exptype = factor (followers);
		 exptype = logical(exptype);
		 fprintf(output,"pop eax\n");
		 fprintf(output,"not eax\n");
		 fprintf(output,"push eax\n");
	 }
	 else exptype = NULL;
	 return exptype;
 }
 void multop (unsigned operation /* код операции */,
                   TYPEREC *exptype   /* указатель на дескриптор типа
                                                    результата операции */)
{ 
    switch ( operation )
    { 
		case andsy: 
			fprintf(output,"and ebx, eax");
        case star: 
			fprintf(output,"imul ebx\n");
				break;
			break;
        case slash :  
            fprintf(output,"mov edx, 0\n");
            fprintf(output,"idiv ebx\n");
			break;
        case modsy :  
            fprintf(output,"mov edx, 0\n");
            fprintf(output,"idiv ebx\n");
            fprintf(output,"mov eax, edx\n");
			break;
      //  case divsy :  . . .
    }
}

 //Слагаемое
TYPEREC* composed (unsigned *followers)
 {
	 TYPEREC* ex1type = NULL, *ex2type; //Типы операндов
	 unsigned operation;
	 if (!belong(symbol, st_termfact))
	 {
		 Error(6);
		 skipto2(st_termfact, followers);
		 nocode=1;
	 }
	 ex1type = factor (followers); //Проверка типа множителя
         if ( !nocode )
		 {
            fprintf(output,"pop ebx\n");
            fprintf(output,"pop eax\n");
		 }
	 while (belong(symbol, op_mult)) //Если символ - операции умножения/деления и т.п.
	 { 
		 operation = symbol; //Запоминаем операцию
		 nextsym();
		 ex2type = factor (followers); //Разбираемся с типами
         if ( !nocode )
		 {
            fprintf(output,"pop ebx\n");
            fprintf(output,"pop eax\n");
		 }
		 ex1type = test_mult(ex1type, ex2type);//, operation); //Проверка корректности
         if ( !nocode )
		 {
			 multop ( operation, ex1type );
			 fprintf(output,"push eax\n");
		 }
	
	 }
	 return ex1type;
 }

//Простое выражение
 TYPEREC* SimpleExpression (unsigned *followers)
 {
	 TYPEREC* ex1type, *ex2type;
	 unsigned operation, sign = 0;
	 if ((symbol == minus) || (symbol == plus)) { sign = 1; nextsym(); } //Проверка знака
	 ex1type = composed (followers);	 //Узнаем тип простого выражения
	 if(sign) //Был ли знак
		 right_sign(ex1type);
	 while (belong(symbol, op_add)) //Проверка на принадлежность символа к операциям сложения/вычитания и т.п.
	 {
		 operation = symbol; //Запоминаем операции
		 nextsym(); 
		 ex2type = composed (followers); //Узнаем тип второго выражения
		 ex1type = test_add(ex1type, ex2type);//, operation);
		 if (!nocode)
		 {
			 fprintf(output,"pop ebx\n");
			 fprintf(output,"pop edx\n");
			 switch (operation) {
				case plus:
					fprintf(output,"add edx, ebx\n");
					break;
				case minus:
					fprintf(output,"sub edx, ebx\n");
					break;
				case orsy:
					fprintf(output,"or edx, ebx\n");
					break;
			 }
			 fprintf(output,"mov eax, edx\n");
			 fprintf(output,"push eax\n");
		 }
	 }
	 return ex1type;
 }

 //Функция programme. Создает фиктивную область действия и проверяет программу
 void programme()
 {
	//Организация фиктивной области действия
	//Открываем стек области действия - localscope->firstlocal
	open_scope();
	//Создаем логический тип
	booltype = newtype(ENUMS);
	
	//Добавляем в его дескриптор константы
	SearchInTable("false"); 
	//SearchInTable находит идентификатор в таблице,
	//Заносит значение hash-функции в hashresult, адрес в таблице имен - в addrname.
	//newident изменяет копию localscope->firstlocal
	localscope->firstlocal = newident(localscope->firstlocal, hashresult, addrname, CONSTS);
	//CreatedNode - переменная, обозачающая текущую вершину. Ту, которую мы создали
	//Функцией newident
	//Теперь зададим в нем ссылку на дескриптор типа
	CreatedNode->idtype = booltype;
	//Добавим константку false в дескриптор логисечкого типа
	booltype->casetype.firstconst = newcons(booltype->casetype.firstconst, addrname);
	CreatedNode->casenode.constvalue.boolval = 0;
	//Сделаем то же самое с true
	SearchInTable("true");
	localscope->firstlocal = newident(localscope->firstlocal, hashresult, addrname, CONSTS);
	CreatedNode->idtype = booltype;
	booltype->casetype.firstconst = newcons(booltype->casetype.firstconst, addrname);
	CreatedNode->casenode.constvalue.boolval = 1;
	//Создадим остальные основные типы
	chartype = newtype(SCALARS);
	stringtype=newtype(STRINGS);
	inttype = newtype(SCALARS);


	//Создаем дескриптор типа integer
	SearchInTable("integer");
	//Создадим идентификатор для integer, т.к. мы уже нашли addrname и hashresult
	localscope->firstlocal = newident(localscope->firstlocal, hashresult, addrname, TYPES);
	//Вставим в этот идентификатор ссылку на тип Integer (inttype)
	CreatedNode->idtype = inttype;

	//Это - константа. 
	SearchInTable("maxint");
	//Создадим идентификатор
	localscope->firstlocal = newident(localscope->firstlocal, hashresult, addrname, CONSTS);
	//Запишем ее тип
	CreatedNode->idtype = inttype;
	//Создадим значение в ней
	CreatedNode->casenode.constvalue.intval = 32767;

	//Далее все аналогично операциям с Integer
	SearchInTable("char");
	localscope->firstlocal = newident(localscope->firstlocal, hashresult, addrname, TYPES);
	CreatedNode->idtype = chartype;

	SearchInTable("string");
	localscope->firstlocal = newident(localscope->firstlocal, hashresult, addrname, TYPES);
	CreatedNode->idtype = stringtype;

	SearchInTable("boolean");
	localscope->firstlocal = newident(localscope->firstlocal, hashresult, addrname, TYPES);
	CreatedNode->idtype = booltype;


	SearchInTable("read");
	localscope->firstlocal = newident(localscope->firstlocal, hashresult, addrname, PROCS);
	CreatedNode->idtype = NULL;
	CreatedNode->casenode.proc.io = 1;

	SearchInTable("readln");
	localscope->firstlocal = newident(localscope->firstlocal, hashresult, addrname, PROCS);
	CreatedNode->idtype = NULL;
	CreatedNode->casenode.proc.io = 2;

	SearchInTable("write");
	localscope->firstlocal = newident(localscope->firstlocal, hashresult, addrname, PROCS);
	CreatedNode->idtype = NULL;
	CreatedNode->casenode.proc.io = 3;

	SearchInTable("writeln");
	localscope->firstlocal = newident(localscope->firstlocal, hashresult, addrname, PROCS);
	CreatedNode->idtype = NULL;
	CreatedNode->casenode.proc.io = 4;

	//Фиктивная область создана. Теперь откроем область 
	open_scope();
	output=fopen("output.asm","w");
    fprintf(output,"extern printf\n");
    fprintf(output,"extern scanf\n");
    fprintf(output,"extern strcmp\n");
    fprintf(output,"extern strcat\n");
    fprintf(output,"extern strcpy\n\n");
    fprintf(output,"extern malloc\n");
    fprintf(output,"extern memcpy\n");
    fprintf(output,"extern free\n\n");
	//Начало проверки
	accept (programsy);
	accept (ident);
	accept (semicolon);
	block (blockfol);
	
	if (!nocode)
	{
		fprintf(output,"mov esp, ebp\n");
		fprintf(output,"pop ebp\n");
		fprintf(output,"ret\n");
		fclose(output);
	}
	else remove("output.asm");
	accept(point);
	while (ch!=endoffile) ch=nextch();
 }﻿
