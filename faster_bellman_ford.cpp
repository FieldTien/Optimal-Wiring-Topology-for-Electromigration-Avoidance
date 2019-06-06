#include <iostream>
#include <fstream>
#include <iostream>
#include <vector>
#include <cstdlib>
#include<algorithm>
#include <list>
#include <cmath>

using namespace std;
class Node
{
	public:
		int x; int y; int capacity;
		friend class Graph_FlowNetWorks;
		friend class edge;
		Node(int x,int y,int ca):x(x),y(y),capacity(ca){};
};
class edge
{
	public:
	    edge *next;  int from,to,flow,capacity,cost;
		friend class Graph_FlowNetWorks;
		friend class LinkedList;
		edge(){cost=0;flow=0;capacity=0;next=0;}
		edge(int cost,int capacity,int from,int to,int flow) {
		
	      this -> cost=cost;
	      this -> capacity = capacity;
	      this -> from = from;
	      this -> to = to;
		  this -> flow = flow;
	      this -> next = 0;
	   }
	   void print()
	   {
	   	cout<<"(c:"<<cost<<",f:"<<flow<<",c:"<<capacity<<") ";
	   }
};

class Graph_FlowNetWorks{
	public: 
	vector<Node*> source;
	vector<Node*> sink;
	int total_flow=0;
	vector<edge> heap;
	int num_node;
	void count_total()
	{
		for(int i=0;i<source.size();i++)
		{
			total_flow+=source[i]->capacity;
		}	
	}
	void construct_sort()
	{
		int cost,capacity;
	    for(int i=0;i<source.size();i++)
	    {
	    	for(int j=0;j<sink.size();j++)
		    {
		    	cost=abs(source[i]->x-sink[j]->x)+abs(source[i]->y-sink[j]->y);
		    	capacity=min(abs(source[i]->capacity),abs(sink[j]->capacity));
		    	heap.push_back(edge(cost,capacity,i,j,0));
			}
		}
		sort(heap.begin(), heap.end(), [](const edge& lhs, const edge& rhs) {
        return lhs.cost < rhs.cost;});
	}
	void push_back_source(int x,int y,int ca){source.push_back(new Node(x,y,ca));}
	void push_back_sink(int x,int y,int ca){sink.push_back(new Node(x,y,ca));}
    void init_graph(edge** graph,edge** re_graph_f,edge** re_graph_b)
	{
		int source_remain;  int sink_remain;  int max=total_flow; int sour_size=source.size();
		int to=0;  
	    for(int i=0; i<heap.size(); i++)
	    {
	    	to=heap[i].to+sour_size;
	    	if(max>0)
	    	{
	    		source_remain=source[heap[i].from]->capacity;
				sink[heap[i].to]->capacity= abs(sink[heap[i].to]->capacity);
				sink_remain=sink[heap[i].to]->capacity;
				
				if (source_remain>0 and sink_remain>0)
				{
					if(heap[i].capacity<=source_remain and heap[i].capacity<=sink_remain)
					{
						if(max >= heap[i].capacity)
						    heap[i].flow=heap[i].capacity;
						else if (max > 0 and max < heap[i].capacity)  
						    heap[i].flow=max;  
					}
					else
					{
						if(max >= min(source_remain,sink_remain))
						    heap[i].flow=min(source_remain,sink_remain);
						else if (max > 0 and max < min(source_remain,sink_remain))  
						    heap[i].flow=max;   
					}
					max=max-heap[i].flow;
					source[heap[i].from]->capacity=source[heap[i].from]->capacity-heap[i].flow;
					sink[heap[i].to]->capacity=sink[heap[i].to]->capacity-heap[i].flow;
			    	re_graph_f[heap[i].from][heap[i].to]=edge(heap[i].cost,heap[i].capacity-heap[i].flow,heap[i].from,to,0) ;
			    	re_graph_b[heap[i].to][heap[i].from]=edge(-heap[i].cost,-heap[i].flow,to,heap[i].from,0); 
				    graph[heap[i].from][heap[i].to]=heap[i];
					graph[heap[i].from][heap[i].to].to=to;
				}
				else
				{
					
					re_graph_f[heap[i].from][heap[i].to]=edge(heap[i].cost,heap[i].capacity,heap[i].from,to,0); 
					re_graph_b[heap[i].to][heap[i].from]=edge(-heap[i].cost,0,to,heap[i].from,0);
					graph[heap[i].from][heap[i].to]=heap[i];
					graph[heap[i].from][heap[i].to].to=to;
				}
		    }
		    else
		    {
				re_graph_f[heap[i].from][heap[i].to]=edge(heap[i].cost,heap[i].capacity,heap[i].from,to,0) ;
				re_graph_b[heap[i].to][heap[i].from]=edge(-heap[i].cost,0,to,heap[i].from,0) ;
		    	graph[heap[i].from][heap[i].to]=heap[i];
				graph[heap[i].from][heap[i].to].to=to;
			}
	    }
	    
	    return;	
	}
	void update_graph(edge** graph,edge** re_graph_f,edge** re_graph_b,int from,int to,int min_flow)
	{
		int sour_size=source.size();
		if(from<sour_size)                 // the current edge is source
		{
			to-=source.size();
			re_graph_b[to][from].capacity=re_graph_b[to][from].capacity-min_flow;	
			graph[from][to].flow=graph[from][to].flow+min_flow;
			re_graph_f[from][to].capacity=re_graph_f[from][to].capacity-min_flow;
		}
		else      // current edge is sink
		{
			from-=source.size();
            re_graph_b[from][to].capacity=re_graph_b[from][to].capacity+min_flow;
            graph[to][from].flow=graph[to][from].flow-min_flow;
			re_graph_f[to][from].capacity=re_graph_f[to][from].capacity+min_flow;
		}
		return;
	}
	void update_cycle(edge** graph,edge** re_graph_f,edge** re_graph_b,vector<int> track)
	{
		int sour_size=source.size();
		int min_flow=10000000;
		if (track[num_node]==-1) return;
		int detect=track[num_node];
		int cost=0;
		for (int i=0;i<track[num_node+1];i++)
		{
			if (detect<sour_size)
			{
				cost+=re_graph_f[detect][track[detect]-sour_size].cost;
				min_flow=min(min_flow,abs(re_graph_f[detect][track[detect]-sour_size].capacity));
			}
			else
			{
				cost+=re_graph_b[detect-sour_size][track[detect]].cost;
				min_flow=min(min_flow,abs(re_graph_b[detect-sour_size][track[detect]].capacity));
			}
			detect=	track[detect];
		}
		
		//cout<<"cost is: "<<cost<<"\n";
		detect=track[num_node];
		for (int i=0;i<track[num_node+1];i++)
		{
			update_graph(graph,re_graph_f,re_graph_b,detect,track[detect],min_flow);
			detect=	track[detect];
		}
		
		return;
	}	
};
vector<int> bellman_ford(edge** re_graph_f,edge** re_graph_b,int source_size,int sink_size,vector<int> track)
{
    int min_new;  int size=source_size+sink_size; int T; int detect=-1; int z=0;
    
	for(int i=0;i<size+2;i++) track[i]=-1;
    vector<int> memo_old(size,0); 
    vector<int> memo_new(size,0); 
    bool end=0; int go_break=1; bool next_break=0;  int record=0;
	for(int i=1;i<=size;i++)
	{
		
//		if(end==1) break;
		detect=-1;
	   	for(int j=0;j<size;j++)
	   	{
	   		record=-1;
	   		min_new=10000;
	   		if(j<source_size)
   			{
		   		for(int z=0;z<sink_size;z++)
		   		{
		   			if(re_graph_f[j][z].capacity!=0)
		   			{
		   				
		   				T=memo_old[re_graph_f[j][z].to]+re_graph_f[j][z].cost;
		   				if(T < memo_old[j] and  T< min_new)
			   			{
			   				
			   				    
			   				

							detect=j;
							record=z+source_size;
			   				
			   				min_new=T;
			   				memo_new[j]=min_new;	
						}
					}
				}
				if(record!=-1)
				{
					if(next_break==1 and track[j] == record) 
					{
						end=1;
						break;
					}
					track[j]=record;
					
					
				}
				
				
			}
	   		else
	   		{
	   			for(int z=0;z<source_size;z++)
		   		{
		   			if(re_graph_b[j-source_size][z].capacity!=0)
		   			{
		   				T=memo_old[re_graph_b[j-source_size][z].to]+re_graph_b[j-source_size][z].cost;
		   				if(T < memo_old[j] and T < min_new)
			   			{
			   				detect=j;
			   				track[j]=z;
			   				min_new=T;
			   				memo_new[j]=min_new;
						}
					}
				}
			}
			if(track[j]==-1)
			{
				go_break++;
			}
			if(end==1) break;
		}
		if(detect==-1) break;
		if(end==1) break;
		if(go_break==0) next_break=1;
		go_break=0;
		memo_old=memo_new;
	} 
	
    
	if(detect==-1)
	{
		return track; 
	}   
	
	z=0;
	int hash[size]={0};
	hash[detect]=z;
    //cout<<detect<<" ";
	int repeat=0;
	for(int i=0;i<size;i++)
	{
		//cout<<track[detect]<<" ";
		z++;
		if(hash[track[detect]]!=0)  
		{
			track[size]=track[detect];        //repeat assign
			track[size+1]=z-hash[track[detect]];
			break;
		}
		else
		{
			detect=track[detect];
			hash[detect]=z;
		}
	} 
	//cout<<"\n";
	return track;  
}

void print_graph(edge** graph,int row_number,int column_number)
{
	for(int i=0;i<row_number;i++)
	{
		for(int j=0;j<column_number;j++)
		{
			graph[i][j].print();  
			if(j==column_number-1) cout<<"\n";
		}
	}
}
	 

int main(int argc, char** argv)
{
	Graph_FlowNetWorks net;
	ifstream myfile (argv[1]);
    myfile>>net.num_node;
    int temp[3]={0};
    for(int i=0;i<net.num_node;i++)
    {
    	myfile>>temp[0]; myfile>>temp[1]; myfile>>temp[2]; 
    	if(temp[2]>0)
    	{
    		
    		net.push_back_source(temp[0],temp[1],temp[2]);
		}
		else if(temp[2]<0)
		{
			net.push_back_sink(temp[0],temp[1],temp[2]);
		}
	}
    myfile.close();
	
	
	net.construct_sort();
	net.count_total();
	int source_size=net.source.size(); int sink_size=net.sink.size();
	    
	edge** graph=new edge*[source_size];
    for(int i=0;i<source_size;i++)graph[i]=new edge[sink_size];   
	edge** re_graph_f=new edge*[source_size];
    for(int i=0;i<source_size;i++)re_graph_f[i]=new edge[sink_size];  
	edge** re_graph_b=new edge*[sink_size];
    for(int i=0;i<net.sink.size();i++)re_graph_b[i]=new edge[source_size];
	net.init_graph(graph,re_graph_f,re_graph_b);
    vector<int> track(source_size+sink_size+2, -1); 
	track=bellman_ford(re_graph_f,re_graph_b,source_size,sink_size,track);

    int i=0;
    while(track[net.num_node] != -1)
    {
    	i++;
    	net.update_cycle(graph,re_graph_f,re_graph_b,track); 
    	track=bellman_ford(re_graph_f,re_graph_b,source_size,sink_size,track);
    	cout<<"iteration :"<<i<<"\n";
	}
	vector<int> node(source_size*sink_size,-1);  
	int traverse=0;
	int total=0;
	for(int i=0;i<source_size;i++)
	{
	    for(int j=0;j<sink_size;j++)
		{
		      if(graph[i][j].flow!=0)
		      {
		      	//node[traverse]=i*source_size+j;
		      	traverse++;
		      	total=total+(graph[i][j].flow*graph[i][j].cost);
		      	
		      	
			  }
		} 
	} 

    ofstream outfile (argv[2]);
    outfile << total<<"\n";
    cout<<total<<"\n";
	for(int i=0;i<source_size;i++)
	{
	    for(int j=0;j<sink_size;j++)
		{
		      if(graph[i][j].flow!=0)
		      {
      			
				outfile <<net.source[graph[i][j].from]->x<<" "<<net.source[graph[i][j].from]->y<<" "
				<<net.sink[graph[i][j].to-source_size]->x<<" "<<net.sink[graph[i][j].to-source_size]->y<<" "<<graph[i][j].flow<<"\n";	      	
		      	
			  }
		} 
	} 
	
    return 0;
}
