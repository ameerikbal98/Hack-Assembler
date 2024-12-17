#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <utility>


//misc funtions--------------------------------------------------------------------------------------------------------------------------------------------------------------------
//Trim any leading or trailing spaces that may exist in the code.
std::string trim_space(std::string str)
{
    std::string ret_val;
    for(int i = 0; i < str.length();i++)
    {
        if(str[i] == ' ')
            continue;
        ret_val.push_back(str[i]);
    }
    return ret_val;
}

bool check_digit(std::string str)
{
    for(int i = 0; i < str.length(); i++)
    {
        if(!isdigit(str[i]))
        {
            return false;
        }
    }

    return true;
}



//Label managing---------------------------------------------------------------------------------------------------------------------------------------------------------------


class label
{
    std::map<std::string,std::string> labels;

public:
    void enter_label(std::string key, std::string value)
    {
        std::pair<std::string,std::string> p1;
        p1.first = key;
        p1.second = value;
        labels.insert(p1);

    }

    bool check_label_exist(std::string key)
    {
        if(labels.find(key) == labels.end())
        {
            return false;
        }
        else
        {
            return true;
        }
    }

    std::string get_value(std::string st1)
    {
        return labels[st1];
    }


};




//Managing A instructions----------------------------------------------------------------------------------------------------------------------------------------------------
//converts a number to binary

class a_instructions
{
    label l;
    int largest_mem;
    

public:
    a_instructions()
    {
        l.enter_label("SCREEN",num_to_16bit_hack_instruction(16384));
        l.enter_label("KBD",num_to_16bit_hack_instruction(24576));
        l.enter_label("THAT",num_to_16bit_hack_instruction(4));
        l.enter_label("THIS",num_to_16bit_hack_instruction(3));
        l.enter_label("ARG",num_to_16bit_hack_instruction(2));
        l.enter_label("LCL",num_to_16bit_hack_instruction(1));
        l.enter_label("SP",num_to_16bit_hack_instruction(0));
        largest_mem = 16;

    }
    std::string to_binary(int n)
    {
        if (n == 0) return "0";
        else if (n == 1) return "1";
        else if (n%2 == 0) return (to_binary(n/2) + "0");
        else if (n%2 == 1) return (to_binary(n/2) + "1");
    }

//converts the a instruction number to 16 bit hack instruction
    std::string num_to_16bit_hack_instruction(int num)
    {
        std::string bin = to_binary(num);
        std::string zeros;


        //pads the bits to 15 bits
        if(bin.length()!=15)
        {
            for(int i = bin.length();i<15;i++)
            {
                zeros.append("0");
            }
        }

        //appending the 16th bit to signify it is an 'A' instruction
        zeros.append("0");
        bin = zeros + bin;
        return bin;

    }

    void insert_loop_labels(std::string str,int num)
    {
        l.enter_label(str,num_to_16bit_hack_instruction(num));
    }

//takes in the a instruction and process it to remove @ and R symbols
    std::string a_instruction(std::string stro)
    {
        std::string str;
        for(int i = 1;i<stro.length();i++)
        {
            str.push_back(stro[i]);
        }
        if(!check_digit(str))
        {
            if(l.check_label_exist(str))
            {
                return l.get_value(str);
            }
            if(str[0] == 'R')
            {
                std::string tst = str.substr(1,str.length()-1);
                if(check_digit(tst))
                {
                    int c = atoi(tst.c_str());
                    if(c<16)
                    {
                        l.enter_label(tst,num_to_16bit_hack_instruction(c));

                        return num_to_16bit_hack_instruction(c);  
                    }
                }
                else
                {
                    l.enter_label(str,num_to_16bit_hack_instruction(largest_mem));
                    largest_mem++;
                    return num_to_16bit_hack_instruction(largest_mem-1);
                    
                }
            }
            else
            {
                l.enter_label(str,num_to_16bit_hack_instruction(largest_mem));
                largest_mem++;
                return num_to_16bit_hack_instruction(largest_mem-1);
            }
            
        }
        else
        {
            int num = atoi(str.c_str());
            
            l.enter_label(str,num_to_16bit_hack_instruction(num));
            return num_to_16bit_hack_instruction(num); 
        }
    }

    int get_largest_mem()
    {
        return largest_mem;
    }
};



//Managing C instructions------------------------------------------------------------------------------------------------------------------------------------------------------
struct out_dest
{
    bool D = false;
    bool M = false;
    bool A = false;
};


class c_instruction{
private:
    out_dest destination;
    std::map<std::string,std::string> op_codes {{"0","0101010"},{"1","0111111"},{"-1","0111010"},{"D","0001100"},{"A","0110000"},
                                                {"M","1110000"},{"!D","0001101"},{"!A","110001"},{"!M","1110001"},{"-D","0001111"},
                                                {"-A","0110011"},{"-M","1110011"},{"D+1","0011111"},{"A+1","0110111"},{"M+1","1110111"},
                                                {"D-1","0001110"},{"A-1","0110010"},{"M-1","1110010"},{"D+A","0000010"},{"D+M","1000010"},
                                                {"D-A","0010011"},{"D-M","1010011"},{"A-D","0000111"},{"M-D","1000111"},{"D&A","0000000"},
                                                {"D&M","1000000"},{"D|A","0010101"},{"D|M","1010101"}};
    std::map<std::string,std::string> jmp_codes{{"NULL","000"},{"JGT","001"},{"JEQ","010"},{"JGE","011"},{"JLT","100"},{"JNE","101"},{"JLE","110"},{"JMP","111"}};
    
public:
    void enable_destination(char c)
    {
        
        switch(c)
        {
            case 'A':
                destination.A = true;
                break;
            case 'D':
                destination.D = true;
                break;
            case 'M':
                destination.M = true;
                break;
            default:
                std::cerr << "wrong destination";
        }
    }
    std::string return_dest_bits()
    {
        std::string bits;
        bits+= destination.A == false?"0":"1";
        bits+= destination.D == false?"0":"1";
        bits+= destination.M == false?"0":"1";
        return bits;
    }

    std::string return_bit_code_op(std::string key)
    {
        key = trim_space(key);
        return op_codes[key];
    }

    std::string return_bit_code_jmp(std::string key)
    {
        key  = trim_space(key);
        return jmp_codes[key];
    }

};






std::string read_cinstruction(std::string code)
{
    std::string instruction;
    c_instruction co;
    bool is_dest = false;

    int i = 0;
    for(int j = 0; j < code.length(); j++)
    {
        if(code[j] == '=')
        {
            is_dest = true;
            break;
        }
    }
     
    if(is_dest == true)
    {
        while(code[i]!='=')
        {
            co.enable_destination(code[i]);
            i++;
            
        }
        i++;
    }
    
    
    std::string op_string;
    while(i < code.length() && code[i]!=';')
    {
        op_string.push_back(code[i]);
        i++;
    }
    

    std::string jmp_string;

    if(code[i] == ';')
    {
        i++;
        while(i<code.length())
        {
            jmp_string.push_back(code[i]);
            i++;
        }
    }
    else
    {
        jmp_string = "NULL";
    }

    std::string final_code;

    

    final_code = "111" + co.return_bit_code_op(op_string) + co.return_dest_bits() + co.return_bit_code_jmp(jmp_string);

    return final_code;
}



//string cleaning and classifications----------------------------------------------------------------------------------------------------------------------------
//clean lines from in line comments;
std::string clean_string(std::string st)
{
    std::string temp;
    for(int i = 0; i < st.length(); i++)
    {
        if(st[i]!='/')
        {
            temp.push_back(st[i]);
        }
        else
        {
            break;
        }
    }
    return temp;
}

//classify instructions into comments, C instruction and A instructions
char classify_instruction(std::string st)
{
    if(st[0] == '@')
    {
        return 'A';
    }
    else if(st[0] == '/')
    {
        return 'E';
    }
    else
    {
        return 'C';
    }
}



int main()
{
    std::ifstream file("Pong.asm");
    std::string line;
    std::string a_bit;
    std::ofstream fileo("Pong.hack");
    a_instructions ains;

    int line_count = 0;
    while(file.peek() != EOF)
    {
        bool flag = false;
        std::getline(file,line);
        if(line.empty())
            continue;
        line = trim_space(line);
        char c = classify_instruction(line);
        if(c == 'E')
        {
            continue;
        }
        line = trim_space(line);
        std::string temp_string;
        if(line[0] == '(')
        {
            for(int i = 1; line[i]!=')';i++)
            {
                std::cout << line_count << std::endl;
                temp_string.push_back(line[i]);
            }
            flag = true;
        }
        
        ains.insert_loop_labels(temp_string,line_count);
        if(!flag)
            line_count++;
    }
    file.clear();
    file.seekg(0);

    while(file.peek() != EOF)
    {
        std::getline(file,line);
        if(line.empty()||line[0] == '(')
            continue;
        line = trim_space(line);
        std::cout << line;
        char c = classify_instruction(line);
        if(c == 'E')
        {
            continue;
        }

        std::string b = clean_string(line);
        if(c == 'A')
        {
            a_bit = ains.a_instruction(b);
        }
        else if(c == 'C')
        {
            a_bit = read_cinstruction(b);
        }
        fileo << a_bit <<"\n";
    }
    
}