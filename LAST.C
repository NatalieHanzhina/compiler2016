#include <stdlib.h>
#include <stdio.h>
/* */

#define TYPES      121   
#define CONSTS     122   
#define VARS       123   
#define PROCS      124   
#define FUNCS      125  
typedef struct typerec TYPEREC;
int labelCounter=0;
#define REFERENCE 1 /*Local variable or parameter-value*/
#define REGISTER  2 /* register */
#define CONSTANT  3 /* CONSTANT*/
#define REF_VAR   4 /* Referenced variable */

unsigned nocode;
FILE *output;


//////////////////////////////////////////////////////////////////////////////////////////////////////////
extern void Error(unsigned);

void block(unsigned *followers);
union const_val /* Constant value*/
{
	int intval      /* Integer or boolean type */ ;
	int boolval;
//	char *charval   /* char */;
};

struct idparam
 { unsigned parhash; /*The value of the Hash function*/
   char *patidn; /* A reference to the name in the table of names */
   TYPEREC *Type; /* Information about the type of the parameter */
   int mettransf; /* The transmission method */
   unsigned par_offset;/* Parameter offset relative to the beginning of the data area */   
   struct idparam *linkparam; /* Link to information about the next parameter */
 };

struct treenode
{
	unsigned hashvalue   /* The value of the Hash function */;
	char *idname        /* The name in the name table */ ;
	unsigned clas      /* the way of using */;
    TYPEREC  *idtype; /* Pointer to type descriptor */
	union
	{  /* For constants*/
		union const_val constvalue; /* Constant value */
        /* For procedures (functions) */
        struct
        {
			struct idparam *param /* Pointer to information about the parameters */;
			int forw /* Forward description information */;
			int io; /*1-read, 2-readln, 3-write, 4-writeln*/
        } proc;
		struct
		{ 
			unsigned staticlevel;
			int offset;
			unsigned param_var /* =TRUE,  If the variable is a parameter passed by reference */;
		} vars;
	} casenode;

	struct treenode *leftlink ;
	struct treenode *rightlink;
}*CreatedNode = NULL, *LocalTree = NULL;
typedef struct treenode NODE;
//Type descriptor structure:
union variapart   /*Variant part of the descriptor*/
{ 
		TYPEREC  *basetype; /* Pointer to base type */
        struct reestrconsts  *firstconst;  /* Pointer to the first element of the constants list */

};  

struct reestrconsts  /* Structure of an enumeration type constant*/
{
	char *addrconsts; /* Address of the identifier in the table of names */
	struct reestrconsts *next; /* Pointer to the following structure*/
};



struct typerec
{
	struct typerec *next;  /* A pointer to the next type descriptor */
	unsigned typecode;  /* Type code */
	union variapart casetype;  /* Variant part of the descriptor */
} *booltype, *inttype, *chartype, *stringtype, *vartype, *ConstantType = NULL;

//This is a localscope scope descriptor structure 
struct scope
{
	struct treenode *firstlocal;
	TYPEREC *typechain;
	struct scope *enclosingscope;
	int count_locals /* Size of the data scope */;
	int level;
	int localvar;
} *localscope;
typedef struct scope SCOPE;
int level=0;
//Open a new scope
void open_scope()
{
	SCOPE *newscope;
	newscope = (SCOPE*)malloc(sizeof(SCOPE));
	newscope->firstlocal = NULL;
	newscope->typechain = NULL;
	newscope->enclosingscope = localscope;
	newscope->count_locals=0;
	newscope->level=level++;
	newscope->localvar=0;
	localscope = newscope;	
};
void dispose_types( TYPEREC* Item ) { /* Deleting a Type Table */
  TYPEREC* PrevItem;
  while(Item!=NULL)
   { PrevItem=Item; 
     Item=Item->next; 
     free( (char*) PrevItem);
   };
};
void dispose_ids(struct treenode* Root) { /* Deleting the Identifiers Table */
   if(Root!=NULL)
   { dispose_ids(Root->leftlink); 
     dispose_ids(Root->rightlink); 
     free((char*) Root); 
   };
}
//Closing the scope
void close_scope()
{
	//localscope = localscope->enclosingscope;
	SCOPE *oldscope; 
	oldscope=localscope;
	localscope=localscope->enclosingscope;  /* Deleting the Identifiers Table */
	dispose_ids( oldscope->firstlocal);  /* Deleting a Type Table */
	dispose_types( oldscope->typechain);  
	free( (char *) oldscope);
}

//Entering a new type descriptor into a type table
TYPEREC *newtype(int tcode)
{
	struct typerec *new1;
	new1 = (struct typerec*)malloc(sizeof(struct typerec));
	new1->typecode = tcode;
	new1->next = localscope->typechain;	
	switch (new1->typecode)
	{
		case SCALARS:
			break;
		case ENUMS:
			new1->casetype.firstconst = NULL;
			break;
	};
	localscope->typechain = new1;
	return(new1);
}

//Adding a new identifier to the Identifiers  table
//The CreatedNode variable is referenced to the created item
NODE *newident(NODE *Tree,unsigned hashfunc,char *addrname,int classused)
{
	NODE *tree;
	int flag = 0;
	tree = Tree;
	//We search for the current identifier in this scope.
	//its presence corresponds to the variable flag
	while ((tree != NULL) && (flag != 1))
	{
		
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
		//If you did not find it - look for a tree. Depending on the hash function.
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
	//If the variable exists, then we output an error
// and return a reference to this identifier
	if (flag == 1)
	{
		
		Error(101); 
		nocode=1;
		//return Tree;
	}
	//If not found
	else
	{
		//Come to the end and insert
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
		//Did not find - go further recursively
		else
			if (hashfunc < Tree->hashvalue)
			{
				Tree->leftlink =newident(Tree->leftlink,hashfunc,addrname,classused);
			}
			else
			{
				Tree->rightlink =newident(Tree->rightlink ,hashfunc,addrname,classused);
			}
		//return Tree; //Return this tree
	}
	return Tree; 
};

typedef struct reestrconsts LIST;

// Add a new constant to the descriptor of the enumerated type.
// insert to the beginning
LIST* newcons(LIST *List, char *addrname)
{
	LIST *nov;
	nov = (LIST *) malloc(sizeof(LIST));
	nov->addrconsts = addrname;
	nov->next = List;
	return nov;
}

//Auxiliary list item
struct listrec
{
	struct treenode *id_r;
	struct listrec *next;
}*varlist;

// Look up the identifier in the table. It gives an error if it is not there
// And if there is, then returns a reference to it
NODE *SearchIdent (SCOPE* local,char *addrname,unsigned hashfunc)
{
	int flag=0;
	SCOPE* current = local;
	NODE* Tree = local->firstlocal;
	//Search by element tree in all scopes of action
	while (flag == 0)
	{
		Tree = current->firstlocal;
		while ((Tree != NULL) && (flag != 1)) 
		{
			if ((Tree->idname == addrname))
			{
				flag = 1; //Found
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
		//Transition to another scope of action if not found in this one
		if (current->enclosingscope != NULL) 
			current = current->enclosingscope;
		else break;
	}
	//If the item is not found - we output an error that the name is not described
	if (flag == 0)
	{
		
		return NULL;
	}
	//else
		//Return a reference to this identifier
		return Tree;
}
void bssSection(NODE* tree)
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
 }
// Add a new variable to the list of variables that wait for the specified type
// (when vardeclaration described all the variables, and the type was written only at the end
// then they are all put in the queue and at the end get the type
void newvariable()
{
	//Again just inserting to the top of the list
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

//\DD\F2\EE \EA\E0\EA \F0\E0\E7 Assigning a type to all variables waiting in the queue
void addattributes(int flag)
{
	 struct listrec *listentry, *oldentry;
	 listentry = varlist;
	 
	 while (listentry!=NULL)
	 {
		 listentry->id_r->idtype = vartype;
		if (localscope->level==1)
		{
		 if (!nocode)
			bssSection(listentry->id_r);
		 
		}
		else  if (flag)
			listentry->id_r->casenode.vars.offset=-(listentry->id_r->casenode.vars.offset);
		 oldentry = listentry;
		 listentry = listentry->next;
		 //clear the element of the queue after it has been written
		 free((void *) oldentry);
	 }
}

//Type Compatibility Check
 int Compatible(TYPEREC* f, TYPEREC* s)
 {
	if ((s == NULL) || (f == NULL)) return 0; //This is if one of the types is not correct, then leave
	if (f == s) 
		return 1;
	return 0;
 }

 TYPEREC* simpletype();
 TYPEREC* arraytype();
 TYPEREC* expression ();
 void operatore();
 TYPEREC* SimpleExpression();

 // The IsConstant function returns 1 if the character is a numeric constant.
// Otherwise, returns 0. A reference to a constant type descriptor is added to the COnstantType variable.
 int IsConstant(unsigned smb)
 {
    if (smb == stringc)
	{
		ConstantType = stringtype; //Return a descriptor of type real
		return 1;
	}
	if (smb == intc)
	{
		ConstantType = inttype;	//Return a descriptor of type int
		return 1;
	}
	if (smb == charc)
	{
		ConstantType = chartype;	//Return a descriptor of type char
		return 1;
	}
	return 0;
 }
// The "type" function checks the syntax and semantics of the type declaration, returning a reference to
 // descriptor of this type
 TYPEREC* type (unsigned *followers)
{
	//This is a type descriptor that we will return
	TYPEREC* TypeEntry = NULL;

	if(!belong (symbol, st_typ))
	{
		Error(18);
		skipto2(st_typ, followers);
		nocode=1;
	}
	////Return a reference to a simple type descriptor
	else TypeEntry = simpletype (followers);

	if (!belong (symbol, followers))
	{
		Error(6);
		skipto1(followers);
		nocode=1;
	}
	//Return Value
	return TypeEntry;
}

 //Simpletype function. Analysis of a simple type. Returns a reference to its descriptor
 TYPEREC* simpletype (unsigned *followers)
 {
	 NODE* Ident = NULL;//, *Ident2 = NULL;
	 TYPEREC* TypeEntry = NULL;
	 //Neutralization of syntax errors
	 if (!belong (symbol, st_simpletype))
	 {
		 Error (18);
		 skipto2(st_simpletype, followers);
		 nocode=1;
	 }
	 //Syntactic and semantic analysis
	 if (symbol == ident) 
	 {
		 //Find this identifier in our table
		 Ident = SearchIdent(localscope, addrname, hashresult);
		 if (Ident==NULL) {Error(104);nocode=1;}
		 nextsym ();
			 //If it's just an identifier
			 if ((Ident != NULL) && (Ident->clas != TYPES))
			 {
				 Error(100);
				 nocode=1;
			 }
		 //Return value
		 if ((Ident != NULL)) TypeEntry = Ident->idtype;
		 else TypeEntry = NULL;

	 }
	 //Neutralization of syntax errors
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
	//If we have found the description of variables
	if (symbol == ident)
	{
		varlist = NULL; //Initialize the queue of variables that expect a type
		newvariable();  //We put the current variable there
		localscope->localvar+=4;
		nextsym();
		while (symbol == comma)
		{
			nextsym();
			newvariable(); //Adding variables in the waiting list
			accept(ident);
			localscope->localvar+=4;
		}
		accept(colon);
		vartype = type(followers); //Find out the type of all these variables
		addattributes(1); //Assign to all of them this type

		if (!belong (symbol, followers))
		{
			Error(6);
			skipto1(followers);
			nocode=1;
		}
	}
}

 //Array description processing
 //TYPEREC* arraytype(unsigned *followers)
 //{
	//TYPEREC* TypeEntry = newtype(ARRAYS); //Create a new array type descriptor
	//struct indextyp* ptr = NULL;

	//accept (arraysy); 
	//accept (lbracket); 

	////We initialize its variant part. Selecting memory
	//TypeEntry->casetype.arraytype.indextype = (struct indextyp*)malloc(sizeof(struct indextyp*));
	////We enter the type of the first index in an array type descriptor
	//TypeEntry->casetype.arraytype.indextype->Type = simpletype (af_simpletype);

	////We put a pointer to the first index of the array
	//ptr = TypeEntry->casetype.arraytype.indextype;
	////
	//ptr->next = NULL;

	////While a comma, read the indexes 
	//while (symbol == comma)
	//{
	//	nextsym();
	//	//We allocate memory for the following index
	//	ptr->next = (struct indextyp*)malloc(sizeof(struct indextyp*));
	//	//We write this type into an array type descriptor
 //       ptr->next->Type = simpletype (af_simpletype);
	//	//We translate the pointer and the next do NULL
	//	ptr = ptr->next;
	//	ptr->next = NULL;
	//}

	//accept (rbracket); 
	//accept (ofsy);
	//
	////The type of elements that make up the array is read, as it should, at the end
	//TypeEntry->casetype.arraytype.basetype = type(followers);
	//return TypeEntry;
 //}
 
//Description of the section of variables. Here only syntax analysis.
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


	
 }
int varval;
struct idparam *funclistend;
void newparam() 
{
	//Completes the list of parameters
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

void addpartyp() { //Uses funclist and writes a type to the Parameter Table
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
		varlist = NULL;//Start of the parameter list
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
		addattributes(0);
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
				//nextsym();
				//if(!belong(symbol,idstarters))
				//{
				//	Error(2);
				//	skipto2(idstarters,followers);
				//}
				//if(symbol==ident)
				//{
				//	nextsym();
				//}
				accept(functionsy);
				//Ident = SearchIdent(localscope, addrname, hashresult);	
				//if (Ident!=NULL)
				//	if (Ident->casenode.proc.forw)
				//		typevar = (Ident->idtype);
				//	else Error(101);
				//else
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
						//ent=newident(hashresult,addrname,VARS); ????
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
						fprintf(output,"sub esp, %d\n",  localscope->localvar + 4);
						fprintf(output,"mov esp, ebp\n");
						fprintf(output,"pop ebp\n");
						if (localscope->count_locals)
							fprintf(output,"ret %d\n",localscope->count_locals-localscope->localvar);
						else
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

//Block. Syntax analysis only.
 void block (unsigned *followers)
 {
	unsigned ptra[SET_SIZE];
	if (!nocode)
	{
		if (localscope->level==1)
		{
			fprintf(output,"section .data\n");
			fprintf(output,"int_format_s_n dd \"%%d\", 10, 0\n");
			fprintf(output,"int_format_p_n db \"%%d\", 10, 0\n");
			fprintf(output,"int_format_s dd \"%%d\", 0\n");
			fprintf(output,"int_format_p db \"%%d\", 0\n");
			fprintf(output,"section .bss\n");
		}
	}
	SetDisjunct(followers, begpart, ptra);
	varpart (begpart); //ptra?????
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
			fprintf(output,"\nmain:\n");
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
			if (symbol == leftpar)
			{
			//	accept(leftpar);
				 do
				 {
					nextsym();
					// typevar = variable(ptra);
				
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
							if (localscope->level==1)
								fprintf(output,"lea ebx, [ebp - %d]\n", Ident->casenode.vars.offset);
							else
								fprintf(output,"lea ebx, [ebp + %d]\n", -Ident->casenode.vars.offset+localscope->count_locals-localscope->localvar+8);
							fprintf(output,"push ebx\n");
							fprintf(output,"push int_format_s\n");
							fprintf(output,"call scanf\n");
							fprintf(output,"add esp, 8\n");
							fprintf(output,"popa\n");
						}
				 } while (symbol == comma);
		//		 accept(rightpar);
			}
			//accept(semicolon);
			break;
		case 2://readln
			if (symbol == leftpar)
			{
			//	accept(leftpar);
				 do
				 {
					nextsym();
					// typevar = variable(ptra);
				
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
							if (localscope->level==1)
								fprintf(output,"lea ebx, [ebp - %d]\n", Ident->casenode.vars.offset);
							else
								fprintf(output,"lea ebx, [ebp + %d]\n", -Ident->casenode.vars.offset+localscope->count_locals-localscope->localvar+8);
							fprintf(output,"push ebx\n");
							fprintf(output,"push int_format_s_n\n");
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
							fprintf(output,"push int_format_p\n");
							fprintf(output,"call printf\n");
							fprintf(output,"add esp, 8\n");
						}
				} while (symbol == comma);
			//	accept(rightpar);
			}
	//		accept(semicolon);
			break;
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
							fprintf(output,"push int_format_p_n\n");
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
	 TYPEREC *typevar=NULL, *h_ptr;
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
						 }	
						 if (!Compatible(h_ptr, typevar))
						 {
							 Error(189);
							 nocode=1;
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
			 if (ptrparam!=NULL) //If the number of formal parameters is not equal to the fact parameters 
			 {
				 Error(126);
				 nocode=1;
			 }
			 
		 }
	 }
 }
 //Operator.
 void operatore (unsigned *followers)
 {
	 int localLabelCounter = ++labelCounter;
	 int llC;
	 TYPEREC* exptyp;	//Type of expression
	 NODE* cond = NULL /*Condition identifier*/, *caseid = NULL /**/; 
	 struct textposition currentpos, exprpos; //Position in the text (to get the error in the right place)
	 unsigned ptra [SET_SIZE];

	 if (!belong (symbol, st_statement))
	 {
		 Error(6);
		 skipto2(st_statement, followers);
		 nocode=1;
	 }
	 //Composite operator. 
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
	 //If statement
	 else if (symbol == ifsy)
	 {

		 SetDisjunct (af_iftrue, followers, ptra);
		 accept (ifsy);

		 exprpos = token; //Remember the position in the text
		 llC=localLabelCounter;
		 localLabelCounter = ++labelCounter;
		 exptyp = expression (ptra); //analyze expression
		 localLabelCounter = ++labelCounter;
		 currentpos = token; //Write the position after the expression
		 if ((exptyp == NULL) || (!Compatible(exptyp, booltype))) //If the type is not compatible with Boolean
		 {
			 token = exprpos; //go back
			 Error(145);	//Report an error
			 token = currentpos; //And back forward
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
		 //++labelCounter;
		 //int localLabelCounter = labelCounter + 1;
		 if (!nocode) 
		 {
			fprintf(output,"pop eax\n");
			fprintf(output,"cmp eax, 1\n");
            localLabelCounter++;
            fprintf(output,"jne L%d\n", llC);
		//	llC=localLabelCounter;
		 }
		// localLabelCounter = ++labelCounter;
		 operatore (ptra);
		 localLabelCounter = ++labelCounter;
		 if (symbol == elsesy)
		 {
			 if (!nocode) 
			 {
				 fprintf(output,"jmp L%d\n", (llC + 1));
				 fprintf(output,"L%d:\n", llC);
				 llC++;
				 
			 }
			 accept (elsesy);
			 operatore (followers);
			 localLabelCounter = ++labelCounter;
		 }
		 fprintf(output,"L%d:\n", llC);
		 labelCounter=localLabelCounter;
	 }
	 //Loop Operator with Parameter
	 //else if (symbol == forsy)
	 //{
		// accept (forsy);
		// //Finding an identifier-index
		// cond = SearchIdent(localscope, addrname, hashresult); 
		// if (cond==NULL) Error(104);
		// //Verify class of use
		// if ((cond != NULL) && (cond->clas != VARS))
		//	 Error(100);
		// accept (ident);
		// accept (assign);
		// SetDisjunct(followers, af_for1, ptra);
		// exprpos = token; //The position is memorized
		// exptyp = expression (ptra); //analyze the type of expression
		// currentpos = token; //We remember the position after expression
		// if ((cond != NULL) && (!Compatible(cond->idtype, exptyp))) //We check its correctness and compatibility
		// {
		//	 token = exprpos;
		//	 Error(145);
		//	 token = currentpos;
		// }

		// if (!belong (symbol, ptra))
		// {
		//	Error(tosy);
		//	skipto1(ptra);
		// }

		// if (symbol == tosy) accept (tosy);
		// else accept (downtosy);
		// SetDisjunct(af_whilefor, followers, ptra);
		// exprpos = token; //Again, the same is to memorize the position in the text to display an error
		// exptyp = expression (ptra); //The analysis of the expression
		// currentpos = token;
		// if ((cond != NULL) && (!Compatible(cond->idtype, exptyp))) //Type compatibility check
		// {
		//	 token = exprpos;
		//	 Error(145);
		//	 token = currentpos;
		// }
		// if (!belong (symbol, ptra))
		// {
		//	Error(dosy);
		//	skipto1(ptra);
		// }
		// accept (dosy);
		// operatore (followers);
	 //}
	 else if (symbol==whilesy)
	 {
        //++labelCounter;
        //int localLabelCounter=labelCounter+1;
		nextsym ();
		SetDisjunct(af_whilefor, followers, ptra);

		llC=localLabelCounter;
		fprintf(output,"L%d:\n",(llC + 1));
		localLabelCounter=++labelCounter;
		exptyp = expression(ptra);
		localLabelCounter=++labelCounter;
		if (exptyp != booltype) {Error (135);nocode=1;}
		accept(dosy);
		if (!nocode)
		{
            fprintf(output,"pop eax\n");
            fprintf(output,"cmp eax, 1\n");
            localLabelCounter++;
            fprintf(output,"jne L%d\n",  llC);
            
		}
		operatore(followers);
		localLabelCounter=++labelCounter;
		if (!nocode)
		{
            //fprintf(output,"pop eax\n");
            //fprintf(output,"cmp eax, 1\n");
            fprintf(output,"jmp L%d\n", (llC + 1));
            fprintf(output,"L%d:\n", llC);
            labelCounter = localLabelCounter;
		}
	 }
	 else if (symbol==repeatsy)
	 {
        //++labelCounter;
        //int localLabelCounter=labelCounter+1;
		nextsym ();
		SetDisjunct(af_repeat, followers, ptra);
		if (!nocode)
		{
			llC=localLabelCounter;
            fprintf(output,"L%d:\n",(llC));
		}
		localLabelCounter=++labelCounter;
		operatore(ptra);
		localLabelCounter=++labelCounter;
		while (symbol == semicolon)	
		{
			nextsym();
			operatore(ptra);
		}
		accept(untilsy);

		exptyp = expression(followers);
		localLabelCounter=++labelCounter;
		if (!nocode)
		{
            fprintf(output,"pop eax\n");
            fprintf(output,"cmp eax, 1\n");
            fprintf(output,"jne L%d\n", (llC));
            labelCounter = localLabelCounter;
		}
		if (exptyp != booltype) {Error (135);nocode=1;}

	 }
	 //Assignment
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
					if ( Ident->clas == VARS)
						if (localscope->level==1)
							fprintf(output,"mov [ebp - %d], eax\n",Ident->casenode.vars.offset);
						else
							fprintf(output,"mov [ebp + %d], eax\n",-Ident->casenode.vars.offset+localscope->count_locals-localscope->localvar+8);
					 //fprintf(output,"push dword, [ebp - %d]\n", Ident->casenode.vars.offset);
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
		 //else
			// Error(100);//StandartProc(followers);//read(ln) or write(ln)
	 }
	if (!belong (symbol, followers))
	{
		Error(6);
		nocode=1;
		skipto1(followers);
	}
 }

 
//Arithmetic operation checks - \F2\F3\EF\EE \EF\E5\F0\E5\E1\EE\F0\EE\EC

 //Checking an expression for belonging to a logical type
TYPEREC* logical(TYPEREC* exptype)
{
	if (exptype == NULL) return NULL;
	if ((exptype == booltype))// || (exptype->typecode = LIMITEDS) && (exptype->casetype.limtype.basetype == booltype)) 
		return booltype;
	Error(145);
	nocode=1;
	return NULL;
}

//Checking the correctness of multiplication
TYPEREC* test_mult(TYPEREC* exptype1, TYPEREC* exptype2)
{
	if ((exptype1 == NULL) || (exptype2 == NULL)) return NULL;
//	if ((exptype1 == realtype) || (exptype2 == realtype)) return realtype;
	if ((exptype1->typecode == SCALARS) && (exptype2->typecode == SCALARS) && (exptype1 != booltype)
		&& (exptype2 != booltype))// && (exptype1 != chartype) && (exptype2 != chartype)) 
			return exptype1;
	Error(145);
	nocode=1;
	return NULL;
}

//Checking the correctness of addition
TYPEREC* test_add(TYPEREC* exptype1, TYPEREC* exptype2)
{
	if ((exptype1 == NULL) || (exptype2 == NULL)) return 0;
//	if ((exptype1 == realtype) || (exptype2 == realtype)) return realtype;
	if ((exptype1->typecode == SCALARS) && (exptype2->typecode == SCALARS) && (exptype1 != booltype)
		&& (exptype2 != booltype)) 
			return exptype1;
	Error(145);
	nocode=1;
	return NULL;
}

//Checking the correctness of the comparison
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

//Checking  the sign before the expression
int right_sign(TYPEREC* exptype)
{
	if (exptype == NULL) return 0;
	return ((exptype == inttype));
}

//Expression
 TYPEREC* expression (unsigned *followers)
 {

	 TYPEREC* ex1type = NULL, *ex2type; //Types of operands
	 unsigned operation; //Operation code
	 int localLabelCounter=++labelCounter;
	 if (!belong (symbol, st_express))
	 {
		 Error(6);
		 skipto2(st_express, followers);
		 nocode=1;
	 }
	 ex1type = SimpleExpression (followers); //Type of first expression
// If the character is a symbol of the comparison operation
	 if (belong(symbol, op_rel))
	 {
		 operation = symbol; 
		 nextsym();			
		 ex2type = SimpleExpression (followers);	//Type of second expression
		 ex1type = test_comparing(ex1type, ex2type,operation); //Compatibility check
	 
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
			break;
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

 //FActor
 TYPEREC* factor (unsigned *followers)
 {
	unsigned ptra [SET_SIZE];
	TYPEREC* exptype = NULL;
	NODE* node; //A pointer to the top of the identifier table for the current identifier
	 if (!belong (symbol, st_termfact))
	 {
		 Error(6);
		 skipto2(st_termfact, followers);
		 nocode=1;
	 }
	 // this is a constant
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
	 // this is an identifier
	 else if (symbol == ident)
	 {
		 // find it
		node = SearchIdent(localscope, addrname, hashresult);
		if (node==NULL) { Error(104); exptype = NULL; nocode=1;}
		//get the type
		else
			switch(node->clas)
		{
			case VARS:
				exptype = node->idtype;//variable(followers); ??????
				nextsym();
				if (!nocode)
					if (localscope->level==1)
						fprintf(output,"push dword [ebp - %d]\n", node->casenode.vars.offset);
					else
						fprintf(output,"push dword [ebp + %d]\n", -node->casenode.vars.offset+localscope->count_locals-localscope->localvar+8);
					//fprintf(output,"push dword %s\n", node->idname);
				break;
			case CONSTS:
				exptype = node->idtype;
				nextsym();
				if (!nocode)
				{
					//switch (node->idname)
					{
					//case "true":
						fprintf(output,"push dword %d\n", node->casenode.constvalue.boolval);
						//break;

					}
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
	 }
	 //left parenthesis
	 else if (symbol == leftpar)
	 {
		 nextsym();
		 //Expression analisys
		 exptype = expression (followers);
		 accept (rightpar);
	 }
	 //if NOT 
	 else if (symbol == notsy)
	 {
		 accept (notsy);
		 //We analyze the expression and check its belonging to logical
		 exptype = factor (followers);
		 exptype = logical(exptype);
		 fprintf(output,"pop eax\n");
		 fprintf(output,"not eax\n");
		 fprintf(output,"push eax\n");
	 }
	 else exptype = NULL;
	 return exptype;
 }
 void multop (unsigned operation /* operation code
 */,
                   TYPEREC *exptype   /* Pointer to the operation result type descriptor */)
{ 

    switch ( operation )
    { 
		case andsy: 
			fprintf(output,"and ebx, eax");
            break;
        case star: 
			fprintf(output,"imul ebx\n");
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

 //term
TYPEREC* composed (unsigned *followers)
 {
	 TYPEREC* ex1type = NULL, *ex2type; //Types of operands
	 unsigned operation;
	 if (!belong(symbol, st_termfact))
	 {
		 Error(6);
		 skipto2(st_termfact, followers);
		 nocode=1;
	 }
	 ex1type = factor (followers); //Checking the type of multiplier

	 while (belong(symbol, op_mult)) //If the symbol is multiplication / division, etc.
	 { 
		 operation = symbol;
		 nextsym();
		 ex2type = factor (followers); //get types
         if ( !nocode )
		 {
            fprintf(output,"pop ebx\n");
            fprintf(output,"pop eax\n");
		 }
		 ex1type = test_mult(ex1type, ex2type); //Compatibility check
         if ( !nocode )
		 {
			 multop ( operation, ex1type );
			 fprintf(output,"push eax\n");
		 }

	
	 }
	 return ex1type;
 }


 TYPEREC* SimpleExpression (unsigned *followers)
 {
	 TYPEREC* ex1type, *ex2type;
	 unsigned operation, sign = 0;
	 if ((symbol == minus) || (symbol == plus)) { sign = 1; nextsym(); } //Sign checking
	 ex1type = composed (followers);	 //get the type of a simple expression
	 if(sign) //if there is a sign then check
		 right_sign(ex1type);
	 while (belong(symbol, op_add)) //Checking whether the character belongs to addition / subtraction, etc.
	 {
		 operation = symbol; 
		 nextsym(); 
		 ex2type = composed (followers); //get the type of the second expression
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

 //Function programme. Creates a fictive scope and checks the program
 void programme()
 {
	//Organization of fictive scope
	//Open the activity scope stack - localscope->firstlocal
	open_scope();
	//Create a Boolean Type
	booltype = newtype(ENUMS);
	
	SearchInTable("false"); 
// SearchInTable finds the identifier in the table,
// Write the value of the hash function in hashresult, the address in the name table is in addrname.
// newident changes the copy of localscope-> firstlocal. And in the CreatedNode variable, newident writes a pointer to the created vertex
	localscope->firstlocal = newident(localscope->firstlocal, hashresult, addrname, CONSTS);
	//CreatedNode - A variable that overrides the current vertex. The one we created by
// Function newident
// Now we specify in it a reference to the type descriptor
	CreatedNode->idtype = booltype;
	//Add the constant false to the logical type descriptor
	booltype->casetype.firstconst = newcons(booltype->casetype.firstconst, addrname);
	CreatedNode->casenode.constvalue.boolval = 0;
	//The same with true
	SearchInTable("true");
	localscope->firstlocal = newident(localscope->firstlocal, hashresult, addrname, CONSTS);
	CreatedNode->idtype = booltype;
	booltype->casetype.firstconst = newcons(booltype->casetype.firstconst, addrname);
	CreatedNode->casenode.constvalue.boolval = 1;
	//create the other basic types
	chartype = newtype(SCALARS);
	stringtype=newtype(STRINGS);
	inttype = newtype(SCALARS);

	//Create an integer descriptor
	SearchInTable("integer");
	//create an identifier for integer. We already found addrname and hashresult
	localscope->firstlocal = newident(localscope->firstlocal, hashresult, addrname, TYPES);
	//Insert in this identifier a reference to the type Integer (inttype)
	CreatedNode->idtype = inttype;
	
	SearchInTable("maxint");
	//Create an identifier
	localscope->firstlocal = newident(localscope->firstlocal, hashresult, addrname, CONSTS);
	//write its type
	CreatedNode->idtype = inttype;
	//create a value
	CreatedNode->casenode.constvalue.intval = 32767;

	//Further all is similar to operations with Integer
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
	//CreatedNode->casenode.proc.param=funclistend;

	//The active scope is created. Now open the scope
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
	//Start of analisys
	accept (programsy);
	accept (ident);
	accept (semicolon);
	block (blockfol);
	accept(point);
	if (!nocode)
	{
		fprintf(output,"mov esp, ebp\n");
		fprintf(output,"pop ebp\n");
		fprintf(output,"ret\n");
		fclose(output);
	}
	else 
		remove("output.asm");
	while (ch!=endoffile) nextch();
 }
