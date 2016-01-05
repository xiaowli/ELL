//// test.cpp
//

//#include "layers.h"
//#include "MutableDecisionTree.h"
//#include "QpLayoutGenerator.h"
#include "CommandLineParser.h"
//#include <vector>
//#include <memory>
//#include <iostream>
//#include <fstream>
//#include <sstream>
//#include <string>
//
//using namespace decision_tree;
//using namespace layers;
//using namespace tree_layout;
//using std::make_shared;
//using std::ifstream;
//using std::ofstream;
//using std::cerr;
//using std::endl;
//using std::string;
//using std::stringstream;
using std::cout;
using std::cerr;
using std::endl;

using namespace utilities;



// A struct of parameters
struct app_params
{
    int numIter = 0;
    double thresh = 0.0;
};

// A subclass of your parameter struct that knows how to Add its members to the commandline parser
struct ParsedParams : app_params, utilities::ParsedArgSet
{
    ParsedParams(CommandLineParser& parser) : ParsedArgSet(parser)
    {
        AddArgs(parser);
    }

    virtual void AddArgs(utilities::CommandLineParser& parser)
    {
        parser.AddDocumentationString("---- Iteration-related parameters ----"); // optional documentation string that gets printed out when you use the --help option
        parser.AddOption(numIter, "numIter", "i", "Number of iterations", 0);
        parser.AddOption(thresh, "thresh", "t", "Threshold", 0.01);
    }

	virtual CommandLineParser::ParseResult PostProcess(CommandLineParser& parser)
	{
		if (numIter <= 0)
		{
			return "Number of iterations must be > 0";
		}

		if (thresh > 1.0)
		{
			return "Threshold must be <= 1.0";
		}

		return true;
	}
};

int main(int argc, char* argv[])
{
	CommandLineParser cmdline(argc, argv);

	// Add a plain variable to the parser
	string filepath;
	cmdline.AddDocumentationString("---- General app parameters ----");
	cmdline.AddOption(filepath, "filepath", "f", "Output filepath", "");

	// add parsed arg set
	ParsedParams testArgs(cmdline);

	// Now actually parse the arguments and set the corresponding parameter values
	try
	{
		cmdline.ParseArgs();
	}
	catch (const CommandLineParser::PrintHelpException&)
	{
		exit(0);
	}
	catch (const CommandLineParser::ParseErrorException& ex)
	{
		std::cerr << "Parsing failed" << std::endl;
		for (const auto& result : ex.GetParseErrors())
		{
			std::cerr << result.GetMessage() << std::endl;
		}
		exit(0);
	}

	std::cout << "filepath: " << filepath << std::endl;

//
//    cout << "numIter: " << Params.numIter << endl;
//    cout << "thresh: " << Params.thresh << endl;
//
//    if (filepath == "")
//    {
//        cout << "Error: filepath not set!" << endl;
//        throw std::runtime_error("filepath not set");
//    }
//
//    cout << "filepath: " << filepath << endl;
//
//
//
//    // open file
//    ofstream fs(filepath);
//
//    // check that it opened
//    if (fs.bad())
//    {
//        cerr << "error writing to file " << filepath << endl;
//        return 1;
//    }
//
//    try
//    {
//        MutableDecisionTree tree;
//        tree.SplitLeaf(0, 0, -11, 11, 13);
//        tree.SplitLeaf(1, 0, -21, 21, 23);
//        tree.SplitLeaf(2, 0, -31, 31, 33);
//        tree.SplitLeaf(3, 0, -41, 41, 43);
//        tree.SplitLeaf(4, 0, -41, 41, 43);
//        tree.SplitLeaf(5, 0, -41, 41, 43);
//        tree.SplitLeaf(6, 0, -41, 41, 43);
//
//        DecisionTreePath path(tree.ChildrenBegin(), tree.ChildrenEnd(), 0);
//        //Scale sc(tree.vertex_begin(), tree.vertex_end());
//
//        QpLayoutGenerator g;
//        auto l = g.generate(path.Children());
//
//        auto col = make_shared<Column>();
//
//        auto L1 = make_shared<Row>();
//        L1->PushBack(make_shared<layers::Constant>(.1));
//        L1->PushBack(make_shared<layers::Constant>(.2));
//        L1->PushBack(make_shared<layers::Constant>(.3));
//        L1->PushBack(make_shared<layers::Constant>(.4));
//        col->PushBack(L1);
//
//        Io::Write(fs, col);
//    }
//    catch (runtime_error e)
//    {
//        cerr << "runtime error: " << e.what() << std::endl;
//    }
//
	return 0;
}

//
//#include "deserializer.h"
//void layers::Deserialize(JsonSerializer& serializer, std::shared_ptr<Mapping>& up)
//{
//    layers::DefaultDeserialize(serializer, up);
//}
//
//
//
