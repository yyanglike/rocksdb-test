//使用c++编写一个基于spirit phoniex，fushion的变量定义和初始化，以及将所有变量插入到一个map中。并编写数学的四则运算，四则运算中可以使用变量。


#include <iostream>
#include <string>
#include <map>
#include <boost/spirit/include/qi.hpp>
#include <boost/fusion/include/std_pair.hpp>
#include <boost/fusion/include/adapt_struct.hpp>
#include <boost/fusion/include/io.hpp>
#include <boost/phoenix/phoenix.hpp>

namespace qi = boost::spirit::qi;
namespace ascii = boost::spirit::ascii;
namespace phoenix = boost::phoenix;

struct Variable {
    std::string name;
    double value;
};

BOOST_FUSION_ADAPT_STRUCT(
    Variable,
    (std::string, name)
    (double, value)
)

template <typename Iterator>
struct VariableGrammar : qi::grammar<Iterator, std::map<std::string, int>(), ascii::space_type> {
    VariableGrammar() : VariableGrammar::base_type(start) {
        using qi::double_;
        using qi::lit;
        using qi::lexeme;
        using ascii::char_;

        start = *(variable >> ';') [phoenix::bind(&VariableGrammar::insert, this, qi::_1, qi::_2)];
        variable = lexeme[+(char_("a-zA-Z"))] >> '=' >> double_;
    }

    void insert(const std::string& str, int value) {
        std::cout << str <<":" << value;
        variables[str] = value;
    }

    std::map<std::string, double> variables;
    qi::rule<Iterator, std::map<std::string, int>(), ascii::space_type> start;
    qi::rule<Iterator, Variable(), ascii::space_type> variable;
};

template <typename Iterator>
struct ExpressionGrammar : qi::grammar<Iterator, double(), ascii::space_type> {
    ExpressionGrammar(const std::map<std::string, double>& variables) : ExpressionGrammar::base_type(expression), variables(variables){
        using qi::double_;
        using qi::char_;
        using qi::lit;
        using qi::_val;
        using qi::_1;
        using qi::lazy;

        var_ref = boost::spirit::lexeme[+(char_("a-zA-Z"))] ;
        factor = double_ | ('(' >> expression >> ')') | var_ref[qi::_val = boost::phoenix::bind(&ExpressionGrammar::get_variable, this, qi::_1)];

        term = factor >> *(char_("*/") >> factor);
        expression = term >> *(char_("+-") >> term);

        factor.name("factor");
        term.name("term");
        expression.name("expression");



        qi::on_error<qi::fail>(expression, std::cout << phoenix::val("Error! Expecting ") << qi::_4 << phoenix::val(" here: \"") << phoenix::construct<std::string>(qi::_3, qi::_2) << phoenix::val("\"") << std::endl);

        qi::debug(expression);
    }
    double get_variable(const std::string& str_name){
        auto it = variables.find(str_name);
        if (it != variables.end()) {
            return it->second;
        } else {
            throw std::runtime_error("Undefined variable: " + str_name);
        }            
    }
    std::map<std::string, double> variables;
    qi::rule<Iterator, double(), ascii::space_type> factor, term, expression;
    qi::rule<Iterator, std::string(), ascii::space_type> var_ref;
};

int main() {
    std::string input = "x = 2.5; y = 3.7;";
    std::map<std::string, double> variables;
    VariableGrammar<std::string::const_iterator> variableGrammar;

    std::string::const_iterator iter = input.begin();
    std::string::const_iterator end = input.end();

    while (iter != end) {
        Variable var;
        bool r = phrase_parse(iter, end, variableGrammar, ascii::space, variables);
        if (r) {
            // variables[var.name] = var.value;
            break;
        } else {
            break;
        }
    }

    std::cout << "Variables:" << std::endl;
    for (const auto& var : variables) {
        std::cout << var.first << " = " << var.second << std::endl;
    }

    std::cout << "Expressions:" << std::endl;
    ExpressionGrammar<std::string::const_iterator> expressionGrammar(variableGrammar.variables);

    input =  "x + y * 2";
    iter = input.begin();
    end = input.end();
    while (iter != end) {
        double result;
        bool r = phrase_parse(iter, end, expressionGrammar, ascii::space, result);
        if (r) {
            std::cout << result << std::endl;
        } else {
            break;
        }
    }

    return 0;
}
