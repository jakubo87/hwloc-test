#include <iostream>
#include <hwloc.h>
#include <vector>
#include <iostream>
#include <string>
#include <utility>
#include <functional>
#include "../include/init_graph.h"
#include "../include/output_graph.h"

///*
//indentation of higher depth opjects
//*/
//std::ostream & operator<<(std::ostream & os,std::pair<std::string,int> p){
//  std::string s;
//  for (int i=0; i<=p.second; ++i){
//    s+=" ";
//  }
//  s+=s+p.first;
//
//  return operator<<(os,s);
//}
//
////general for all types with a std::to_string implementation
//template <class M>
//std::ostream & operator<<(std::ostream & os,std::pair<M,int> p){
//  std::string s;
//  for (int i=0; i<=p.second; ++i){
//    s+=" ";
//  }
//  s+=s+std::to_string(p.first);
//
//  return operator<<(os,s);
//}
//
//
////print topology
//std::ostream & operator<<(std::ostream & os, hwloc_topology_t & t){
//  //std::string s;
//
//  int max_depth=0;
//  int depth=0;
//  std::string s;
//
// //print breadth first
//  os << "breadth first approach\n";
//
//  max_depth = hwloc_topology_get_depth(t);
//
//
//  for (depth = 0; depth < max_depth; depth++) {
//      os << "Objects at level " <<  depth << std::endl;
//      for (unsigned int i = 0; i < hwloc_get_nbobjs_by_depth(t, depth); i++) {
//        auto hw_obj = hwloc_get_obj_by_depth(t, depth, i);
//        s=obj_type_toString(hw_obj) + " " + std::to_string(i) + "\n";
//        s+= "OS_index: " + std::to_string(hw_obj->os_index) + "??? \n";
//        s+= "logical index: " + std::to_string(hw_obj->logical_index);
//        s+='\n';
//        os << std::make_pair(s,depth);
//      }
//    }
//
//  os << "\n";
//
//  return operator<<(os,"");
//}





/*Vertices:
 * VType type aka std::string
 * Index index aka unsigned long int
 * 
 *Edges:
 * EType label aka std::string
 * double weight=0
 *
 *
 *TODO init additional properties by user
 *
 *
 */
int main ()
{
  //#######################   INITIALISATION   ################################################
  //hwloc_init
  hwloc_topology_t t;
  hwloc_topology_init(&t);  // initialization
  hwloc_topology_set_io_types_filter(t, HWLOC_TYPE_FILTER_KEEP_ALL); //for all the devices, or...
  //hwloc_topology_set_io_types_filter(t,HWLOC_TYPE_FILTER_KEEP_IMPORTANT);
  hwloc_topology_load(t);   // actual detection
  //std::cout << t <<"\n"; //print nodes
  
  auto g = init_graph(t);
//TODO expand graph!
  //cleaning up:
  hwloc_topology_destroy(t); //since data was copied hwloc is not needed anymore
 
  //TODO extensible properties or adding properties the boost way... 
  //struct Edge {
  //  EType label;
  //  double weight=0; //TODO remove? 
  //};
  //######################      UPDATING SETUP           #####################################

  //add new relationship: replicatea
  //find last level cache TODO (for all CUs)
  auto l2 = get_vds(g, VType("HWLOC_OBJ_L2CACHE"));
  auto mem = get_vds(g, VType("HWLOC_OBJ_NUMANODE"));
  for (auto vl : l2){
    for (auto vm : mem){
     //this commented line was the former hoped sulotion. however it turned out, that the edges would not appear in the graph. Only like done underneath. This may be some quirk or some larger underlying problem like can be the invalidation of indizes/descriptors and iterators when modifying the graph
     //ED e = add_edge(vl,vm, g); //<-- this alone doesn't work!? TODO
      auto p = boost::add_edge(vl,vm, g);
      auto e = p.first;
      bool ed = p.second;
      put(&Edge::label,g,e, "replicates");
      std::cout << "HEY!!!!! add edge from " << vl << " to "<< vm << "?" << std::endl; // ed <<  std::endl;
    }
  }





  //###################################################################
  //TESTS:
 
  //MATHS
  //combinatorics (to be integrated into finding best solution)
  auto vec = comb(4,std::vector<int> {2,4,6,8,10});
  std::cout << "combining  [2,4,6,8,10] into sets of 4:" << std::endl;
  for(auto co : vec){
    for(auto el : co)
      std::cout << el << " ";
    std::cout << std::endl;
  }
  std::cout << vec.size() << " possible combinations." << std::endl;

  //BASICS:
  //find vd 
  auto vds = get_vds(
      g,                //the graph
      VType("HWLOC_OBJ_CORE"), //the type
      Index(0));               //the index
  std::cout << "vd of core 0: " << vds[0] << std::endl;

  


  //get/set
  const auto e1 = get_ed(g,2,1,"child").front();
  std::cout << "setting the weight of child edge between vertices 2 and 1. Original value: " << get(&Edge::weight, g, e1) << std::endl;
  put(&Edge::weight, g, e1, 1000);
  std::cout << "new value is: " << get(&Edge::weight, g, e1) << std::endl;



  //generic vd query TODO does this also work when queries are generated at runtime...???
  vds = get_vds(g, VType("HWLOC_OBJ_CORE"));
  std::cout << "testing generic querying for vds... CORES have vd: " << std::endl;
  for (auto& v : vds)
    std::cout << v << " ";
  std::cout << std::endl;
  std::cout << "VDs with rubbish type: " << std::endl;
  vds = get_vds(g, VType("xxx"));
  for (auto& v : vds)
    std::cout << v << " ";
  std::cout << std::endl;
  //  vds = get_vds(g, 1); <- does not compile you have to write the explicit type
  std::cout << "searching for VD with type: CORE and index: 1 (testing matching in reverse order): " << std::endl;
  vds = get_vds(g, Index(1), VType("HWLOC_OBJ_CORE"));
  for (auto& v : vds)
    std::cout << v << " ";
  std::cout << std::endl;


  
  //find edge properties
  std::cout << "ed label from vd 1 to 2: " << get_edge_label(g, get_ed(g,1,2,"parent").at(0)) << std::endl;
  //find edges by their property
  std::cout << "List all edges with the parent property:" << std::endl;
  auto eds = get_eds(g, "parent");
  for (const auto& ed : eds){
    std::cout << boost::source(ed,g) << " to " << boost::target(ed,g) << std::endl;
  }


  //GROUPS
  //make group - at first arbitrary
  std::vector<VD> vs = {10,12};
  auto i = make_group("Group1", vs, g);
  std::cout << " Group1 has vd: " << i << std::endl; 


  //VISITORS TODO
  count_obj<Index>(g);
  accumulate<Index>(g);
  //accumulate(g);

  //###### PATHS/patterns #########
  //return the group members of group 
  std::cout << "The members of Group1 are the following:" << std::endl;
  //TODO make path queries like "is connected to group", or "is 'child' of cache" 


  //find subgraphs
  find_pattern(g); //TODO

  ///NOTE!!!! in order to calculate properties simply make a subgraph and use std::accumlate... for instance...



  //DISTANCES
  //calc custom distance
  //define a distance function:
  std::function<double(VD,VD,const graph_t&)> dist1 =  [&](auto va, auto vb, const graph_t& g)
    {
      //the result, if the graph has no direct edge in any allowed category defined by this function
      double res = NOPATH;  //default 
      auto range = boost::out_edges(va, g); //TODO boost::out_edges works, if you "make clean" one in a while...!
      //check all edges for label "child"
       std::for_each (range.first, range.second,[&](const auto & ei){
        if (g[ei].label=="child" && vb==target(ei,g))
          res = 10.0;       //case of rising in the hierarchy
      });
      //check all edges for label "parent"
      std::for_each (range.first, range.second,[&](const auto & ei){
        if (g[ei].label=="parent" && va==source(ei,g) && vb==target(ei,g))
          res = 0.0;        //case when descending in hierarchy
      });

      return res;
    };


  std::cout << "distance (5,7): " << distance(5,7,g, dist1) << std::endl;
  std::cout << "distance (6,7): " << distance(6,7,g, dist1) << std::endl;

  std::cout << "distance (8,9): " << distance(8,9,g, dist1) << std::endl;
  std::cout << "path from 9 to 8:" << std::endl; 
  auto r1 = shortest_path(g, 8, 9, dist1); 
  for (auto vd : r1)
    std::cout << vd << " ";
  std::cout << std::endl;

  //this returns the shortest distance found over multiple hops - or a direct edge - with respect to only a given distance function
  std::cout << "combined distances from 8 to:" << std::endl;
  for(long unsigned int i=0; i < boost::num_vertices(g); ++i){
    std::cout << i << ": " ;
    std::cout << find_distance(g,8,i,dist1) << std::endl;
  }
  
  //return clostest vertices of specified type sorted by distance
  auto cl_pus = find_closest_to(g, dist1, "HWLOC_OBJ_PU", 11);
  std::cout << "closest PUs relative to vd(11) with respect to user defined function dist1:" << std::endl;
  for (auto a : cl_pus){
    std::cout << a.first << " " ;
  }
  std::cout << std::endl;
  std::cout << "with distances (respectively):" << std::endl;
  for (auto a : cl_pus){
    std::cout << a.second << " " ;
  }
  std::cout << std::endl;



  //TODO find partitioning

  //ancestry iterator
  std::cout << "is 3 an ancestor of 11?: " << is_ancestor(11,3,g) << std::endl;
  std::cout << "is 11 an ancestor of 3?: " << is_ancestor(3,11,g) << std::endl;



  //TODO return subgraph
  make_dotfile_nolabel(g,"totalnl.dot");
  make_dotfile(g,"total.dot");

  //isolate a subgraph and reduce to hwloc relationships
  auto g2 = make_can_tree(g,14);
  make_dotfile_nolabel(g2,"hwloc.dot");

  return 0;
}
//TODO make tests generic
