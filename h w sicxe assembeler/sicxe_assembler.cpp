#include<iostream>
#include<string>
#include<fstream>
#include<sstream>
#include<stdlib.h>
#include<math.h>
#include<iomanip>
using namespace std;
/*********** hex string to decimal int **************/
/*unsigned long int hextodec(string s)
{
    unsigned long int sum=0;
    for(int i=s.length()-1;i>=0;i--)
      {
          int val;
          if(s[i]<='9')
            val=s[i]-'0';
          else if(s[i]=='A'||s[i]=='a')
            val=10;
          else if(s[i]=='B'||s[i]=='b')
            val=11;
          else if(s[i]=='C'||s[i]=='c')
            val=12;
          else if(s[i]=='D'||s[i]=='d')
            val=13;
          else if(s[i]=='E'||s[i]=='e')
            val=14;
          else if(s[i]=='F'||s[i]=='f')
            val=15;
          sum=sum*16+val;
      }
    return sum;
}*/
/************** Struct Definitions ******************/
struct SymbolTable
{
	string label;
	int address;
};
SymbolTable symtab[100];//100 is arbitrary memory allocation
struct Opcode_element
{
    string mnemonic;
    int format;//can be 3 or 4
    int machinecode;
};
struct Record{//not used till yet 24/9 18:29
	unsigned short int line_no;
	unsigned short int Loc;
	unsigned long int ObjectCode;
	//char oc_toprint[8];//if required then use
	string Label;
	string Operator;
	string Operand;
	int base_status;
	int format;
};
Record record[100];
/********* making opcode table *********/
const Opcode_element optab[]={//IF COND TO BE PUT FOR + CASE FOR FROMAT 4
    {   "ADD",  3,  0x18},
    {   "AND",  3,  0x40},
    {  "COMP",  3,  0x28},
    {   "DIV",  3,  0x24},
    {     "J",  3,  0x3C},
    {   "JEQ",  3,  0x30},
    {   "JGT",  3,  0x34},
    {   "JLT",  3,  0x38},
    {  "JSUB",  3,  0x48},
    {   "LDA",  3,  0x00},
    {  "LDCH",  3,  0x50},
    {   "LDL",  3,  0x08},
    {   "LDX",  3,  0x04},
    {   "MUL",  3,  0x20},
    {    "OR",  3,  0x44},
    {    "RD",  3,  0xD8},
    {  "RSUB",  3,  0x4C},
    {   "STA",  3,  0x0C},
    {  "STCH",  3,  0x54},
    {   "STL",  3,  0x14},
    {  "STSW",  3,  0xE8},
    {   "STX",  3,  0x10},
    {   "SUB",  3,  0x1C},
    {    "TD",  3,  0xE0},
    {   "TIX",  3,  0x2C},
    {    "WD",  3,  0xDC},
    {  "ADDF",  3,  0x58},
    {  "ADDR",  2,  0x90},
    { "CLEAR",  2,  0xB4},
    { "COMPF",  3,  0x88},
    { "COMPR",  2,  0xA0},
    {  "DIVF",  3,  0x64},
    {  "DIVR",  2,  0x9C},
    {   "FIX",  1,  0xC4},
    { "FLOAT",  1,  0xC0},
    {   "HIO",  1,  0xF4},
    {   "LDB",  3,  0x68},
    {   "LDF",  3,  0x70},
    {   "LDS",  3,  0x6C},
    {   "LDT",  3,  0x74},
    {   "LPS",  3,  0xD0},
    {  "MULF",  3,  0x60},
    {  "MULR",  2,  0x98},
    {  "NORM",  1,  0xC8},
    {"SHIFTL",  2,  0xA4},
    {   "RMO",  2,  0xAC},
    {"SHIFTR",  2,  0xA8},
    {   "SIO",  1,  0xF0},
    {   "SSK",  3,  0xEC},
    {   "STB",  3,  0x78},
    {   "STF",  3,  0x80},
    {   "STI",  3,  0xD4},
    {   "STS",  3,  0x7C},
    {   "STT",  3,  0x84},
    { "COMPF",  3,  0x88},
    {   "SUB",  3,  0x1C},
    {  "SUBF",  3,  0x5C},
    {  "SUBR",  2,  0x94},
    {   "SVC",  2,  0xB0},
    {   "TIO",  1,  0xF8},
    {  "TIXR",  2,  0xB8}
};
int mod_table[100]={0};int mod_count=0;
/******************************* Searching functions ************************************/
int search_optab(string s)//it returns -1 if not fount otherwise returns the index
{
    int max=sizeof(optab)/sizeof(Opcode_element);
    int toreturn=-1;
    for(int i=0;i<max;i++)
      {
          if(optab[i].mnemonic==s)
            {
            toreturn =i;
            break;
            }
      }
    return toreturn;
}
int search_symtab(string s,int max)//it returns -1 if not fount otherwise returns the index
{
    int toreturn=-1;
    for(int i=0;i<max;i++)
      {
          if(symtab[i].label==s)
            {
            toreturn =i;
            break;
            }
      }
    return toreturn;
}
/*****************************************************************************************/
int main()
{
    int symtab_count=7,rec_count=0;//counter to locate the indices of symtab array
    //INITIAL SYMTAB:
    symtab[0].label="B";
    symtab[0].address=3;
    symtab[1].label="S";
    symtab[1].address=4;
    symtab[2].label="T";
    symtab[2].address=5;
    symtab[3].label="F";
    symtab[3].address=6;
    symtab[4].label="A";
    symtab[4].address=0;
    symtab[5].label="X";
    symtab[5].address=1;
    symtab[6].label="L";
    symtab[6].address=2;
    //=====================
    ifstream filein("input.txt");// the source file
    ofstream fileout("intermediate.txt",ios::out);
    int locctr=0;//location counter : increment accordingly - format 3 or 4
    int start_adr=0,prog_length=0;
    string s,file_name;

    /******reading first line********/
    /*getline(filein,s);///assuming that the very first line is not a comment - 24/9 17:34
    string first_line_word[3];
    inputs>>first_line_word[0]>>first_line_word[1]>>first_line_word[2];
    file_name=first_line_word[0];

    /*********************************/
    cout<<"Pass 1 started.......\n";
    while(getline(filein,s))
    {
        static int b_status=0;
        static int line=0;
        line++;
        if(s[0]=='.')//ignoring coment lines
          continue;
        //if we need comments also in the intermediate file then shift the below line to above "above line"
        //fileout<<setw(4)<<setfill('0')<<hex<<locctr<<"\t"<<s<<endl;//GENERATING INTERMEDIATE FILE
        record[rec_count].line_no=line;
        record[rec_count].Loc=locctr;
        istringstream inputs(s);
        string word[3];
        inputs>>word[0]>>word[1]>>word[2];
        if(word[0]=="END")
          {
              record[rec_count].Loc=locctr;
              record[rec_count].Operator=word[0];
              record[rec_count].Operand=word[1];// it may be present e.g at page57
              rec_count++;
              break;
          }
        //cout<<word[0]<<word[1]<<word[2];
        //if(line_no==1)
        //{
            if(word[1]=="START")//this can occur only in the first line
                {
                    istringstream winp(word[2]);
                    int n;//=hex_to_dec(word[2][0])*16*16*16+hex_to_dec(word[2][1])*16*16+hex_to_dec(word[2][2])*16+hex_to_dec(word[2][3]);
                    winp>>hex>>n;
                    locctr=n;
                    start_adr=n;
                    file_name=word[0];
                    record[rec_count].Loc=locctr;//page 49 shows it should be the new locctr
                    record[rec_count].Label=word[0];
                    record[rec_count].Operator=word[1];
                    record[rec_count].Operand=word[2];
                    record[rec_count].base_status=b_status;
                    rec_count++;
                    continue;//now read the second line
                }
            //else locctr=0;
        //}
        int index;
        if(word[2]!="")//the opcode will be in word[1]//label is there
          {
              record[rec_count].Label=word[0];
              record[rec_count].Operator=word[1];
              record[rec_count].Operand=word[2];
              if(search_symtab(word[0],symtab_count)==-1)// that label is not already in symtab
              {
                  symtab[symtab_count].label=word[0];
                  symtab[symtab_count].address=locctr;
                  symtab_count++;
              }
              else// if the label is already present
              {
                  cout<<"duplication of label: "<<word[0]<<" at line no. :"<<dec<<line<<endl;
                  break;
              }
              string temp=word[1];
              if(word[1][0]=='+')
                {
                    locctr+=4;
                    temp=temp.substr(1);
                    record[rec_count].format=4;
                }
              index=search_optab(temp);
              if(index==-1)
                {
                    if(word[1]=="WORD")
                      locctr+=3;
                    else if(word[1]=="RESW")
                    {
                        int n;
                        istringstream winp(word[2]);
                        winp>>n;
                        //cout<<"before resw "<<locctr<<endl;
                        locctr+=3*n;
                        //cout<<"resw n"<<n<<endl;
                        //cout<<"after resw "<<locctr<<endl;
                    }
                    else if(word[1]=="RESB")
                    {
                        int n;
                        istringstream winp(word[2]);
                        winp>>n;
                        locctr+=n;
                    }
                    else if(word[1]=="BYTE")
                    {
                        string temp=word[2].substr(2,word[2].length()-3);
                        //cout<<"debug: 194 :"<<temp<<endl;
                        int n=temp.length();
                        //cout<<hex<<locctr<<"  "<<locctr+n<<endl;
                        if(word[2][0]=='C')
                            locctr+=n;
                        else if(word[2][0]=='X')
                        {
                            int m=ceil((float)n/2.0);//ceil is used as if half byte is used even then locctr will increment by 1
                            //cout<<m<<endl;
                            locctr+=m;
                        }
                    }
                    else
                    {
                        cout<<"invalid opcode: "<<word[1]<<" at line no. :"<<dec<<line<<endl;
                        break;
                    }
                }
              if(word[1][0]!='+')
                {
                    locctr+=optab[index].format;
                    record[rec_count].format=optab[index].format;
                }
          }
          else// label is not there : just increment the locctr
          {
              if(word[0]=="BASE")
              {
                  b_status=1;
                  continue;
              }
              if(word[0]=="NOBASE")
              {
                  b_status=0;
                  continue;
              }
              record[rec_count].Operator=word[0];
              record[rec_count].Operand=word[1];
              string temp=word[0];
              if(word[0][0]=='+')
                {
                    locctr+=4;
                    temp=temp.substr(1);
                    //record[rec_count].Operator=temp;
                }
              index=search_optab(temp);

              if(index==-1)
                {
                        cout<<"invalid opcode: "<<word[0]<<" at line no. :"<<dec<<line<<endl;
                        break;
                }
              if(word[0][0]!='+')
                {
                    locctr+=optab[index].format;
                }
          }
          if(!record[rec_count].format)
            record[rec_count].format=optab[index].format;
          record[rec_count].base_status=b_status;
          rec_count++;
    }
    prog_length=locctr-start_adr;
    //cout<<hex<<prog_length<<endl;
    ofstream out_symtab("symbol_table.txt");
    out_symtab<<"\nSymtable generated:\n===================\n";
    for(int i=7;i<symtab_count;i++)
      out_symtab<<symtab[i].label<<"\t"<<hex<<setw(4)<<setfill('0')<<symtab[i].address<<endl;
    cout<<"\n1.Symbol table generated\n2.Intermediate file generated\n";
    cout<<"\nPass 1 completed.\n";

/************************** printing intermediate file ******************************************/
    for(int i=0;i<rec_count;i++)
      fileout<<setw(4)<<setfill('0')<<hex<<record[i].Loc<<"\t"<<record[i].Label<<"\t"<<record[i].Operator<<"\t"<<record[i].Operand<<"\n";

/*************************************** Pass 2 from here ***************************************/

    cout<<"\nPass 2 started.......\n";
    //ifstream fin("intermediate.txt");//may read from intermediate file but i have used my record array
    ofstream listfile("listing_file.txt");
    ofstream outfile("objectfile.txt");
    //int prog_counter=start_adr;///************ check this ******************/
    int b_val=0x33;//this value of b will be stored in register B during execution but here i am putting it manually
    for(int i=0;i<rec_count;i++)//reading the record array : just same as reading the intermediate file
    {
        int operand_addr=0,machine_code=0,ni=0,xbpe=0;//this when combined gives objectcode
        if(record[i].Operator=="START")
          {
              //write listing line : uptil now 25/9 10:48 i have thought to write this using loop at the end from my record array
              outfile<<"H^"<<left<<setw(6)<<file_name<<"^"<<right<<setw(6)<<setfill('0')<<hex<<start_adr<<"^"<<setw(6)<<setfill('0')<<hex<<prog_length<<endl;//have to modify printing of file_name
              continue;
          }
        //use it at end

        if(record[i].Operator=="END")// actually no need of it if i run for loop uptil rec_count-1
          break;
        //i have already removed the comments i.e didn't store that in my record array
        int op_index;
        if(record[i].Operator[0]=='+')//i.e format 4
          {
              //record[i].format=4;
              record[i].Operator=record[i].Operator.substr(1);
              op_index=search_optab(record[i].Operator);
              if(op_index!=-1)//i.e found
                {
                  machine_code=optab[op_index].machinecode;
                }
              else
                {
                    cout<<"wrong use of + or invalid opcode "<<record[i].Operator<<" after + sign at line no. :"<<record[i].line_no<<endl;
                    break;
                }
              xbpe+=1;///b=p=0
              ni=3;
              if(search_symtab(record[i].Operand,symtab_count)!=-1)
              {
                  operand_addr=symtab[search_symtab(record[i].Operand,symtab_count)].address;
                  mod_table[mod_count++]=i;
              }
              else if(record[i].Operand[0]=='#')
              {
                  string num=record[i].Operand.substr(1);
                  if(search_symtab(num,symtab_count)==-1)//case of #1234
                  {
                    istringstream inp(num);
                    int n;
                    inp>>n;
                    if(n<=1048575)
                      {
                        ni=1;
                        xbpe=1;
                        operand_addr=n;

                      }
                    else if(n>1048575)
                      {
                        cout<<"Error: Even format 4 is not sufficient at line no. :"<<dec<<record[i].line_no<<endl;
                        break;
                      }
                  }
                  else//case of #operand e.g #LENGTH
                  {
                      ni=1;
                      operand_addr=symtab[search_symtab(num,symtab_count)].address-record[i+1].Loc;
                  }
              }
              else
                {
                  cout<<"Error: in operand "<<record[i].Operand<<" at line no. "<<record[i].line_no<<endl;
                  break;
                }
              record[i].ObjectCode=operand_addr+xbpe*1048576+(machine_code+ni)*16777216;
              record[i].Operator="+"+record[i].Operator;
              //record[i].Operator="+"+record[i].Operator;
              continue;
          }
    else{
          //cout<<"here"<<record[i].Operator<<" format"<<record[i].format<<endl;
        op_index=search_optab(record[i].Operator);
        if(op_index!=-1)//i.e found
        {
            machine_code=optab[op_index].machinecode;
            if(optab[op_index].format==1)
            {
                record[i].ObjectCode=machine_code;
                continue;
            }
            if(optab[op_index].format==2)
            {
                record[i].ObjectCode=machine_code*256;
                string temp=record[i].Operand;
                string r1,r2;
                istringstream inp(temp);
                getline(inp,r1,',');
                getline(inp,r2);
                int sym_index=search_symtab(r1,symtab_count);
                record[i].ObjectCode=record[i].ObjectCode+(symtab[sym_index].address)*16;
                if(r2.length()!=0)
                  {
                      sym_index=search_symtab(r2,symtab_count);
                      record[i].ObjectCode=record[i].ObjectCode+symtab[sym_index].address;
                  }
                /*else
                  record[i].ObjectCode*=16;*/
                continue;
            }
            //default case of format 3 and the rest of the process both format 4 or 3
            int x_flag=0,a_flag=0;
            if (record[i].Operand[0]=='@')               // @ is indirect mode
              {//cout<<"here ni is "<<ni<<"*";

                record[i].Operand=record[i].Operand.substr(1);
                a_flag=1;
              }
            if(record[i].Operand[record[i].Operand.length()-2]==','&&record[i].Operand[record[i].Operand.length()-1]=='X')
            {
                x_flag=1;
                xbpe+=8;
                record[i].Operand=record[i].Operand.substr(0,record[i].Operand.length()-2);
            }
            if(record[i].Operator=="RSUB")
            {
                ni=3;
                xbpe=0;
                operand_addr=0;
            }


            if(record[i].Operand[0]=='#')
              {
                  string num=record[i].Operand.substr(1);
                  if(search_symtab(num,symtab_count)==-1)//case of #1234
                  {
                    istringstream inp(num);
                    int n;
                    inp>>n;
                    if(n>=0&&n<=4095&&optab[op_index].format==3)
                      {
                        ni=1;
                        xbpe=0;
                        operand_addr=n;
                      }
                    else if(n>4095&&optab[op_index].format==3)
                      {
                        cout<<"Error: address can't be given in format 3, use format 4 for "<<record[i].Operator<<" at line no. :"<<dec<<record[i].line_no<<endl;
                        break;
                      }
                    /*else if(n>=4096&&n<=1048575&&record[i].format==4)
                      {
                        ni=1;
                        xbpe=1;
                        operand_addr=n;
                        record[i].Operator="+"+record[i].Operator;
                      }*/

                  }
                  else//case of #operand e.g #LENGTH
                  {
                      ni=1;
                      xbpe=2;
                      operand_addr=symtab[search_symtab(num,symtab_count)].address-record[i+1].Loc;
                  }
              }
            else if (symtab[search_symtab(record[i].Operand,symtab_count)].address-record[i+1].Loc>= -2048 && symtab[search_symtab(record[i].Operand,symtab_count)].address-record[i+1].Loc <= 2047) // PC AFTER instruction has been read
              {  //if(record[i].Operator=="JEQ")
                  //6cout<<"here"<<record[i].Operator<<" at "<<hex<<record[i].Loc<<"  "<<record[i+1].Operator<<" at "<<record[i+1].Loc<<"opnd is "<<record[i].Operand<<"sym val ="<<symtab[search_symtab(record[i].Operand,symtab_count)].address<<endl;
                  //assemble opcode + flags 110010 + 12 bit disp.   // 12 bit 2s comp.
                  ni=3;
                  xbpe+=2;
                  //cout<<"symt val="<<symtab[search_symtab(record[i].Operand,symtab_count)].address<<" ";
                  //cout<<"pc ="<<record[i+1].Loc;
                  operand_addr=symtab[search_symtab(record[i].Operand,symtab_count)].address-record[i+1].Loc;
              }
            else if (record[i].base_status&&symtab[search_symtab(record[i].Operand,symtab_count)].address-symtab[search_symtab("B",symtab_count)].address>=0 && symtab[search_symtab(record[i].Operand,symtab_count)].address-symtab[search_symtab("B",symtab_count)].address<= 4095) // Base relative
              {
                  //assemble opcode + flags 110100 + 12 bit disp.   // Disp = TA - BASE
                  ni=3;
                  xbpe+=4;
                  operand_addr=symtab[search_symtab(record[i].Operand,symtab_count)].address-b_val;//symtab[search_symtab("B",symtab_count)].address;
              }
            else
              {
                if(record[i].Operator!="RSUB")
                  cout<<"Error: instruction adressing error at line no. :"<<dec<<record[i].line_no<<" opcode is "<<record[i].Operator<<endl;
              }
                             // changes nix from 110 to 100
          if(x_flag)
            record[i].Operand=record[i].Operand+",X";
          if(a_flag)
          {
            if(ni==1)
              ni=0;
            else if(ni==3)
              ni=2;
            record[i].Operand="@"+record[i].Operand;
          }
        }
        else if(record[i].Operator=="BYTE")
        {
            string temp=record[i].Operand;
            if(temp[0]=='C')
              {
                  temp=temp.substr(2,temp.length()-3);
                  //string hex_temp;//not used uptill now 25/9 11:33
                  /********************** Error handling *****************/
                  if(temp.length()>3)
                    {
                        cout<<"Error : length of constant out of range for: "<<record[i].Operator<<" at line no. :"<<dec<<record[i].line_no<<endl;
                        break;
                    }
                  /******* To Ask : i think it has been solved now *******/
                  record[i].ObjectCode=0;
                  for(int k=0;k<temp.length();k++)
                    {
                        int n_char=temp[k];
                        //cout<<"here";
                        record[i].ObjectCode=record[i].ObjectCode*16*16+n_char;
                        //cout<<record[i].ObjectCode;
                    }
                cout<<dec<<record[i].line_no<<"\t"<<hex<<record[i].Loc<<"\t"<<record[i].Label<<"\t"<<record[i].Operator<<"\t"<<record[i].Operand<<"\t"<<record[i].ObjectCode<<endl;
                continue;
              }
            else if(temp[0]=='X')
            {
                temp=temp.substr(2,temp.length()-3);
                istringstream inp(temp);
                //cout<<temp;
                int n;
                inp>>hex>>n;
                //cout<<"n="<<n;
                record[i].ObjectCode=n;
                //cout<<record[i].ObjectCode;
                //cout<<record[i].ObjectCode;
                cout<<dec<<record[i].line_no<<"\t"<<hex<<record[i].Loc<<"\t"<<record[i].Label<<"\t"<<record[i].Operator<<"\t"<<record[i].Operand<<"\t"<<record[i].ObjectCode<<endl;
                continue;
            }
        }
        else if(record[i].Operator=="WORD")
        {
            istringstream tempin(record[i].Operand);
            tempin>>record[i].ObjectCode;
        }

        else if(record[i].Operator=="RESW"||record[i].Operator=="RESB")
        {
            continue;
        }
        /******************* Error handling *********************************/
        else
        {
            cout<<"Error: invalid opcode:- "<<record[i].Operator<<" at line no. :"<<dec<<record[i].line_no<<endl;
            break;
        }
        /********************************************************************/
        cout<<dec<<record[i].line_no<<"\t"<<hex<<record[i].Loc<<"\t"<<record[i].Label<<"\t"<<record[i].Operator<<"\t"<<record[i].Operand<<"\t"<<machine_code<<"\t"<<ni<<"\t"<<xbpe<<"\t"<<operand_addr<<endl;
        if(record[i].format==3)
          {
              record[i].ObjectCode=operand_addr+xbpe*4096+(machine_code+ni)*65536;
              if(operand_addr<0)
                record[i].ObjectCode+=4096;
          }
        //else if(record[i].format==4)
    }
  }
    cout<<"\nPass 2 Completed.";
    /********************** Printing listing file ******************************/
    for(int i=0;i<rec_count;i++)
    {
        //if(record[i].format==4)
          //record[i].Operator="+"+record[i].Operator;
        listfile<<hex<<setw(4)<<setfill('0')<<record[i].Loc<<"\t"<<record[i].Label<<"\t"<<record[i].Operator<<"\t"<<record[i].Operand<<"\t";
        //if(!record[i].format)
        //{
            if(record[i].format==1)
              listfile<<setw(2)<<setfill('0')<<hex<<record[i].ObjectCode;
            else if(record[i].format==2)
              listfile<<setw(4)<<setfill('0')<<hex<<record[i].ObjectCode;
            else if(record[i].format==3)
              listfile<<setw(6)<<setfill('0')<<hex<<record[i].ObjectCode;
            else if(record[i].format==4)
              listfile<<setw(8)<<setfill('0')<<hex<<record[i].ObjectCode;
            //else if(record[i].ObjectCode!=0)
            else if(record[i].Operator=="WORD")
              listfile<<setw(6)<<setfill('0')<<hex<<record[i].ObjectCode;
            else if(record[i].Operator=="BYTE")
              listfile<<setw(2)<<setfill('0')<<hex<<record[i].ObjectCode;
        //}
        listfile<<endl;
    }
    /******************** printing object file *************************/
    int start_here,upto_here=0;//these are indices
    for(int i=0;i<rec_count;)
    {
        if(record[i].Operator=="END")
          break;
        while(record[i].ObjectCode==0&&(record[i].Operator!="LDA"||record[i].Operator!="WORD"))
          i++;
        start_here=i;
        upto_here=i;
        while(record[upto_here].Loc-record[start_here].Loc<=30)
          {
              upto_here++;
              if(record[upto_here].ObjectCode==0&&(record[upto_here].Operator!="LDA"||record[upto_here].Operator!="WORD"))
                break;
          }
        if(record[upto_here].Loc-record[start_here].Loc>30)
          upto_here--;
        outfile<<"T^"<<hex<<setw(6)<<setfill('0')<<record[start_here].Loc<<"^"<<setw(2)<<record[upto_here].Loc-record[start_here].Loc;
        for(;i<upto_here;i++)
        {
            if(record[i].format==1)
            outfile<<"^"<<setw(2)<<record[i].ObjectCode;
            else if(record[i].format==2)
            outfile<<"^"<<setw(4)<<record[i].ObjectCode;
            else if(record[i].format==3)
            outfile<<"^"<<setw(6)<<record[i].ObjectCode;
            else if(record[i].format==4)
            outfile<<"^"<<setw(8)<<record[i].ObjectCode;
        }
        outfile<<endl;
    }

    for(int k=0;k<mod_count;k++)
    {
        outfile<<"M^"<<hex<<setw(6)<<setfill('0')<<record[mod_table[k]].Loc+1<<"^05"<<endl;//05 manually because format 4 known
    }
    outfile<<"E^"<<hex<<setw(6)<<setfill('0')<<start_adr<<endl;
    cin.get();
}
