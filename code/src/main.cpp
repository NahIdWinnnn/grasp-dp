/* 
 * File:   Algorithm.cpp
 * Author: A. Herrán
 */

#include <bits/stdc++.h>
#include "algorithm/Algorithm.h"

// Global parameters and random engines
bool            parallel_enabled = true;
vector<Tengine> vengine;

Instance   instance;
Parameters parameters;

string LoadInput   (int argc, const char *argv[], unsigned &seed);
void   LoadInstance(const string& pathInstance);
void   abstract();
void   LogParameters();

int main(int argc, const char *argv[]) {

 // Input arguments
    unsigned seed;
    string pathInstance = LoadInput(argc,argv,seed);
    LoadInstance(pathInstance);

 // Seed for random number generation
    random_device rd;
    for(unsigned p=0; p<omp_get_max_threads(); p++) {
        vengine.emplace_back(rd());
        if(seed!=0) vengine[p].seed((p+1)*seed);
        else        vengine[p].seed(random_device{}());
    }

 // Termination criteria
    unsigned  i      = 0;
    auto      imax   = parameters.ALGtv;
    double    t      = 0.0;
    double    tmax   = parameters.ALGtv;
    // double    tmax   = instance.nV*instance.nK*instance.nT/parameters.ALGtv;
    bool      finish = false;
    auto      start  = clk::now();

    // Abstract and parameters log
    abstract();
    LogParameters();

    if ((parameters.CONm == "2P-GRASP-DP" || parameters.CONm == "2P-HGRASP-DP") and instance.nK > 10) {
        cout << " Auto termination: The DP-based construction methods are not suitable for instances with more than 10 clusters." << endl;
        return 0;
    }

 // Algorithm
    Algorithm algorithm(pathInstance, seed);
    while(!finish) {

     // Algorithm iteration
        algorithm.Iterate();
        i++;
        t = (double)(duration_cast<microseconds>(clk::now()-start)).count()/1e6;

     // Logs and communication at every ALGmr iterations
        if(  parameters.ALGlg!=0) algorithm.PrintLogs(t,i);
        if(i%parameters.ALGmr==0) algorithm.Migrate();

     // Checks the termination criteria
        if((parameters.ALGtc=="iter")&&(i>=imax)) finish = true;
        if((parameters.ALGtc=="tcpu")&&(t>=tmax)) finish = true;

    }

 // Saves the results
    if(parameters.ALGlg>0) { // Saves results to a file
        algorithm.SaveLogs();
    }
    else { // Metric to console for iRace
        double value;
        if(parameters.ALGlg == 0)   // Cost for iRace
            if(instance.type=="h") value = algorithm.GetBest().EvaluateHMP();
            else                   value = algorithm.GetBest().obj;
        else {                      // Area for iRace
            value = algorithm.vtcpu[0]*algorithm.mcost[0][0];
            for(int i=1; i<algorithm.vtcpu.size(); i++) value += (algorithm.vtcpu[i]-algorithm.vtcpu[i-1])*algorithm.mcost[i][0];
        }
        cout << fixed << value << endl;
    }

    Solution solu = algorithm.GetBest();
    if (!solu.Validate()) {
        cout << " Final solution is invalid." << endl;
        exit(1);
    }
    else {
        cout << " Final solution is valid." << endl;
        double final_cost = 0.0;
        for (unsigned k = 0; k < instance.nK; k++) {
            for (int i = 0; i < int(solu.sol[k].size()) - 1; i++) {
                for (int j = i + 1; j < int(solu.sol[k].size()); j++) {
                    final_cost += instance.D[solu.sol[k][i]][solu.sol[k][j]] + instance.D[solu.sol[k][j]][solu.sol[k][i]];
                }
            }
        }
        cout << " Final solution cost: " << fixed << setprecision(10) << solu.obj << " = " << final_cost << endl;
        cout << " Optimization completed in " << fixed << setprecision(2) << t << " seconds" << endl;
    }
    

 // End message
    //if(instance.type=="h") cout << pathInstance << " " << seed << " Cost: " << algorithm.GetBest().EvaluateHMP() << " Optimization completed in " << fixed << setprecision(2) << t << " seconds" << endl;
    //else                   cout << pathInstance << " " << seed << " Cost: " << algorithm.GetBest().obj           << " Optimization completed in " << fixed << setprecision(2) << t << " seconds" << endl;

 // Checks the final solution
    //Solution solu = algorithm.GetBest();
    //solu.Evaluate(false);
    //cout << solu.obj << endl;
    //solu.Evaluate(true);
    //cout << solu.obj << endl;

 // Checks the final solution for the HMP instances
    //cout << algorithm.GetBest().EvaluateHMP() << endl;

    return 0;
}

string LoadInput(int argc, const char *argv[], unsigned &seed) {

    bool input;
    string pathInstance;
    for(int i=1; i<argc; i+=2) {
        input = false;
        if(argv[i][0] == '-' && i + 1 < argc) {
            string key   = argv[i];
            string value = argv[i+1];
            if(key=="--instance")             { input = true; pathInstance      = value; }
            if(key=="--termination_criteria") { input = true; parameters.ALGtc  = value; }
            if(key=="--seed")                 { input = true; seed              = stoi(value); }
            if(key=="--termination_value")    { input = true; parameters.ALGtv  = stod(value); }
            if(key=="--schema")               { input = true; parameters.CONm   = value; }
            if(key=="--logs")                 { input = true; parameters.ALGlg  = stoi(value); if(parameters.ALGlg > parameters.ALGtv) parameters.ALGlg = std::numeric_limits<int>::max(); }
            if(key=="--version")              { input = true; parameters.GRASPv = value; }
            if(key=="--alpha")                { input = true; parameters.GRASPa = stod(value); }
            if(key=="--m")                    { input = true; parameters.GRASPm = stoi(value); }
            if(key=="--block")                { input = true; parameters.GRASPb = stoi(value); }
            if(key=="--delta")                { input = true; parameters.GRASPd = stoi(value); }
            if(key=="--move")                 { input = true; parameters.LSm    = value; }
            if(key=="--efficient")            { input = true; parameters.LSe    = static_cast<bool>(stoi(value)); }
            if(key=="--exploration") {        { input = true; parameters.LSs    = value; }
                if((value!="best")&&(value!="hybrid")&&(value!="first")) {
                    cout << "\n Wrong exploration strategy.";
                    cin.get();
                    exit(1);
                }
            }
            if(key=="--debug") { input = true; 
                if((value=="1") || (value=="true")) parameters.DEBUG = true;
                else if((value=="0") || (value=="false")) parameters.DEBUG = false;
                else { cout << "\n Wrong debug flag. Use 1/0 or true/false."; cin.get(); exit(1); }
            }
        } 
        if(input==false) {
            cout << "\n Wrong input." << argv[i];
            cin.get();
            exit(1);
        }
    }

    return pathInstance;
}


void LoadInstance(const string& pathInstance) {
    
 // Opens the instance file 
    ifstream file;
    file.open(pathInstance);
    if(!file) {
        cout << "\n The instance " << pathInstance << " can not be opened.";
        cin.get();
    }

 // Loads the instance parameters
    string instancetype, ignore;
    file >> instance.type;
    if(instance.type == "p") {
        file >> instance.nV >> instance.nK >> instance.nT;
        instance.W  = matDbl(instance.nV, vecDbl(instance.nT, 0.0));
        instance.D  = matDbl(instance.nV, vecDbl(instance.nV, 0.0));
        instance.WL = matDbl(instance.nK, vecDbl(instance.nT, 0.0));
        instance.WU = matDbl(instance.nK, vecDbl(instance.nT, 0.0));
        for (unsigned i = 0; i < instance.nV; i++) {
            file >> ignore;
            for (unsigned t = 0; t < instance.nT; t++) file >> instance.W[i][t];
            for (unsigned j = 0; j < instance.nV; j++) {
                file >> instance.D[i][j];
                instance.D[i][j] = instance.D[i][j]/2.0;
            }
        }
    }
    else if(instance.type == "t") {
        file >> instance.nV >> instance.nK >> instance.nT;
        instance.W  = matDbl(instance.nV, vecDbl(instance.nT, 0.0));
        instance.D  = matDbl(instance.nV, vecDbl(instance.nV, 0.0));
        instance.WL = matDbl(instance.nK, vecDbl(instance.nT, 0.0));
        instance.WU = matDbl(instance.nK, vecDbl(instance.nT, 0.0));
        matDbl C = matDbl(instance.nV, vecDbl(2, 0.0));
        for(int i=0; i<instance.nV; i++) {
            file >> ignore;
            for(unsigned t=0; t<instance.nT; t++) file >> instance.W[i][t];
            file >> C[i][0] >> C[i][1];
        }
        unsigned decimals = 6;
        double   factor   = pow(10,decimals);
        for(int i=0; i<instance.nV; i++) {
            for(int j=i+1; j<instance.nV; j++) {
                instance.D[i][j] = round(sqrt((pow(C[i][0]-C[j][0],2) + pow(C[i][1]-C[j][1],2)))/2.0*factor)/factor;
                instance.D[j][i] = instance.D[i][j];
            }
        }
    }
    else if(instance.type == "h") {
        double handover;
        file >> instance.nV >> instance.nK >> handover;
        instance.nT = 1;
        instance.W  = matDbl(instance.nV, vecDbl(instance.nT, 0.0));
        instance.D  = matDbl(instance.nV, vecDbl(instance.nV, 0.0));
        instance.WL = matDbl(instance.nK, vecDbl(instance.nT, 0.0));
        instance.WU = matDbl(instance.nK, vecDbl(instance.nT, 0.0));
        for (unsigned k=0; k<instance.nK; k++) file >> ignore >> ignore >> instance.WU[k][0];
        for (unsigned i=0; i<instance.nV; i++) file >> ignore >> ignore >> instance.W[i][0];
        string line;
        getline(file,line);
        while(getline(file,line)) {
            vector<string> tokens = Algorithm::Split(line,' ');
            instance.D[stoi(tokens[1]) - 1][stoi(tokens[2]) - 1] = -stod(tokens[3]);
        }
    }
    else {
        cout << "\n Wrong instance file.";
        cin.get();
        exit(1);
    }

 // Generates the weight limits for each pair (cluster,attribute)
    if((instance.type == "p") || (instance.type == "t")) {
        // unsigned decimals = 2;
        // double   factor   = pow(10,decimals);
        // double   mean, std;
        // for(unsigned t=0; t<instance.nT; t++) {
        //     double sum = 0.0;
        //     for(unsigned i=0; i < instance.nV; i++) sum += instance.W[i][t];
        //     mean = sum/(double)instance.nV;
        //     std = 0.0;
        //     for(unsigned i=0; i<instance.nV; i++) std += pow(instance.W[i][t] - mean, 2);
        //     std = sqrt(std/(double)instance.nV);
        //     for(unsigned k=0; k<instance.nK; k++) {
        //         instance.WL[k][t] = round((mean*((double)instance.nV/(double)instance.nK) - 2.0*std)*factor)/factor;
        //         instance.WU[k][t] = round((mean*((double)instance.nV/(double)instance.nK) + 2.0*std)*factor)/factor;
        //     }
        // }

        for (unsigned k = 0; k < instance.nK; k++) {
            for (unsigned t = 0; t < instance.nT; t++)
                file >> instance.WL[k][t];
            for (unsigned t = 0; t < instance.nT; t++)
                file >> instance.WU[k][t];
        }
    }

 // Closes the instance file
    file.close();

}

void abstract() {
    cout << "\n Structure: There are multiple islands, each containing one or more metaheuristic GRASP algorithms (by default, the code assigns one metaheuristic per island; this cannot be changed in the configuration file and must be modified directly in the code).\n";
    cout << "\n Operation: For each global iteration, one iteration is executed on every island. When an island performs an iteration, it means that all metaheuristics within that island also execute an iteration. When a metaheuristic performs an iteration, it refers to running the entire algorithm process once — including generating a new solution, validating it, and optimizing it.\n";
}

void LogParameters() {
    cout << "\n Algorithm parameters:";
    cout << "\n  - Search method: " << parameters.ALGm;
    cout << "\n  - Termination criteria: " << parameters.ALGtc << " = " << parameters.ALGtv;
    cout << "\n  - Number of islands: " << parameters.ALGni;
    cout << "\n  - Migration rate: " << parameters.ALGmr;
    cout << "\n  - Logs frequency: " << parameters.ALGlg;
    cout << "\n Meta-heuristic parameters:";
    if(parameters.ALGm == "grasp") {
        cout << "\n  - Schema: " << parameters.CONm;
        cout << "\n  - GRASP version: " << parameters.GRASPv;
        cout << "\n  - Alpha: " << parameters.GRASPa;
        cout << "\n  - Number of alphas (m): " << parameters.GRASPm;
        cout << "\n  - Block size: " << parameters.GRASPb;
        cout << "\n  - Delta iterations: " << parameters.GRASPd;
    }
    cout << endl;
}
