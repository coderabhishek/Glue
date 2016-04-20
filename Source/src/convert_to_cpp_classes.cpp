//this function is called from generated cpp_classes function which was called fr_om the main function after functionazing and make_classes class

#include <vector>
#include <string>
#include <string.h>
#include <iostream>
#include <map>

#include "../Header/include/function_struct.h"
#include "../Header/include/source_code.h"
#include "../Header/include/function_struct.h"
//responds for the following function call from generated_cpp_classes()
//convert_to_cpp_classes(itr->first,itr->second,lines,&converted_code,&final_class);

std::string eval_expr_(std::string& s)
{
	std::string evaled = "";
	if(*s.begin()=='"') {
		std::string subject = "";
		std::string::iterator itr = s.begin()+1;
		for(;*itr!='"';++itr)
			subject.push_back(*itr);

		evaled.append("artificial_string(\""+subject+"\")");

		itr++;
		for(;itr!=s.end();++itr)
			evaled.push_back(*itr);
	}
	if(evaled == "")
		evaled  = s;

	return evaled;
}



bool is_symbol_(char c){
	char symbols[]=" =:;(){}[]|,.&+-*/'\n#";

	for(int i=0;symbols[i]!='#';++i)
		if(symbols[i]==c)
			return 1;
	return 0;

}

std::string expr_type_(std::string& expr,std::map< std::string,std::string >& variables)
{	
	if(expr[0]=='"')
		return "std::string";
	if(isdigit(expr[0]))
	{
		for(std::string::iterator i=expr.begin();i!=expr.end();++i)
			if(*i=='.')
				return "double";
		return "long int";
	}
	std::string::iterator itr=expr.begin();
	std::string first_variable;
	while(itr!=expr.end()){
		if(is_symbol_(*itr)&&*itr!='.')
			break;
		first_variable.push_back(*(itr++));
	}
	
	if(variables.count(first_variable))
		return variables[first_variable];	//Implicit type casting to be taken care of later on(small fix)
	else return "";
}


void break_into_words_(std::string& line,std::vector< std::string >& tokens)
{
	int flag=1;	//to check if a symbol has been pushed and tokens.end()-1 isn't a continuing token
	for(std::string::iterator itr=line.begin();itr!=line.end();++itr)
	{   
		if(*itr=='"'){
			tokens.push_back("");
			(tokens.end()-1)->push_back('"');
			while(*(++itr)!='"')
				(tokens.end()-1)->push_back(*itr);
			(tokens.end()-1)->push_back('"');
			continue;
			flag=1;
		}
		if(is_symbol_(*itr))
		{
			if(*itr!=' '&&*itr!='\n')
			{
				std::string emp(1,*itr);	
				tokens.push_back(emp);
			}
			flag=1;
			continue;
		}	
		if(flag)
			tokens.push_back("");
		(tokens.end()-1)->push_back(*itr);
		flag=0;
	}
	
}




void convert_to_cpp_classes(unsigned long int start,
							unsigned long int end,
							std::vector< line_pair >& lines,
							std::string& converted_code,
							class_declaration* final_class,
							std::map< std::string,
							std::string >& variables )
   {

     	// if(start == end)
     	// 	std::cout<<"-------------\n\n\n\n"<<lines[start].first<<"\n\n-----------------"
        std ::vector < std::string > tokens;
        tokens.resize(1);
        break_into_words_(lines[start].first,tokens);//the lines get tokenized,inherited function

        if(tokens[0]=="class")
        {
            final_class->name=tokens[1];

            if(tokens[3]==":")
            	final_class->num_base_func=0;
            else if (tokens[3]=="(")
            {
                 std::vector<std::string>::iterator tokens_itr =tokens.begin()+3;
                 while(*(++tokens_itr)!=")")
                 {
                 	(final_class->base_functions).push_back(*tokens_itr);
                 	(final_class->num_base_func)++;//incremented each time a base func is found
                 	tokens_itr++; //to bypass the commas after each base function

                 }



            }//all the base classes(if any)are noted till here;
        
          }
          /*while(*tokens_itr=="\n")
          	tokens_itr++; //gets rid of all the new lines, infact only one new line char would be there
           */
          	//the above block is now no more required as, tokens no more store '\n'

          /************THE FOLLOWING LINES FILL std::string converted_code *******************/
          	//converted_code.append("{\npublic:\n");

           
             for(unsigned long int i=start;i<=end;)
	            {	
		             tokens.clear();
		              break_into_words_(lines[i].first,tokens);
		              std::vector< std::string >::iterator itr=tokens.begin();

		 if(*itr=="if")
		{

			itr++;
			bool is_not=0;
			if(*itr=="not")
				{
					is_not=1;
					itr++;
				}	
			std::string expr="";
			
			if(is_not)
				expr.append("!");

			while(*itr!=":")
				expr.append(*(itr++));
			
			converted_code.append((size_t)lines[i].second,' ');
			converted_code.append("if(");
			converted_code.append(expr);
			converted_code.append(")\n");
			converted_code.append((size_t)lines[i].second,' ');
			converted_code.append("{\n");
			
			int space_count=lines[i].second,st=i;
			
			while(lines[++i].second>space_count);
				
			std::string snippet;
			convert_to_cpp_classes(st+1,i-1,lines,snippet,final_class,variables);
			converted_code.append(snippet);
			
			converted_code.append((size_t)lines[st].second,' ');
			converted_code.append("}\n");
			//i already incremented while changing lines
			continue;

		}
		
		////NOTE:- Every section puts \n at its end so no need to put one in each section's beginning

		if(*itr=="print")
		{

			itr++;
			converted_code.append((size_t)lines[i].second,' ');
			converted_code.append("std::cout<<");
			int f=1;
			while(itr!=tokens.end())
			{
				if(*itr=="("||*itr==")")
					f=!f;

				if(!f||*itr!=",")
					converted_code.append(*itr);
				itr++;
			}
			converted_code.append(";\n");
			i++;
			continue;
		}




				else if (*itr!="def")	//variable declaration
		{
			std::string v=*itr;
			std::string expr="";
			bool is_function_call=1;
			if(*(itr+1)=="="){
				is_function_call=0;
				itr=itr+2;	//skip = sign
			}
			while(itr!=tokens.end())
				expr.append(*itr++);
    		expr = eval_expr_(expr);

			if(!is_function_call){
				variables[v]=expr_type_(expr,variables);
			
			converted_code.append((size_t)lines[i].second*tab_size,' ');
			if(!variables.count(v))
				converted_code.append(variables[v]);
			converted_code.append(expr_type_(v,variables));	
			converted_code.append(" ");
			converted_code.append(v);
			converted_code.append(" = ");
			    		std::cout<<"Evaled  -- "<<expr<<"\n";

			converted_code.append(expr);
			converted_code.append(";\n");
			}
			else{
				converted_code.append((size_t)lines[i].second*tab_size,' ');
				converted_code.append(expr);
				converted_code.append(";\n");

			}

			i++;
			continue;

		}	



		/***** SOME MORE CASES HAVE TO BE TAKEN CARE OF HERE.*****/
		/***** Variables have to be explicitly taken care off.****/
      
      //At the the end, the closing lines of the classe in c++ is written

    
     }

    //  converted_code.append("\ntalking of this one};\n");
     // the above step to be taken care off in rendering the cpp code
 std::cout<<"printing the converted class code\n"<<converted_code <<"haha\n";

     }//end of convert_to_cpp_classes
