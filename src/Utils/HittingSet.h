/**
 * HittingSet.h
 *
 * 
 *
 * @author : Si-Mohamed Lamraoui
 * @contact : simo@nii.ac.jp
 * @date : 2013/04/01
 * @copyright : NII 2013
 */


#ifndef _HITTINGSET_H
#define _HITTINGSET_H

#include <iostream>
#include <vector>
#include <set>
#include <map>
#include <algorithm>
#include <fstream>
#include <sstream>

#include "yices_c.h"


typedef struct Node {
    unsigned edgeVal;
    std::set<unsigned> set;
    struct Node *pred;
    std::vector<struct Node*> nexts;
} Node_t;


//============================================================================
class HittingSet {

public:
    static void getMinimalHittingSets(std::vector<std::set<unsigned> > &F,
                                      std::vector<std::set<unsigned> > &MHS);
    static void getMinimalHittingSets_LP(std::vector<std::set<unsigned> > &S,
                                  std::vector<std::set<unsigned> > &MHS);
    
private:
    static void computeHittingSetDAG(std::vector<std::set<unsigned> > &F,
                                     Node_t *root, Node_t *node);
    static void printSet(std::set<unsigned> S);
    static void saveTree(Node_t *node, unsigned path[], unsigned len,
                         std::vector<std::set<unsigned> > &MHS_tmp);
    
};
//============================================================================

#endif
