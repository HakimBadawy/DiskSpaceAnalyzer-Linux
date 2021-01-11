#include <sys/types.h>
#include <sys/param.h>
#include <dirent.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <time.h>
#include <stdlib.h>
#include<vector>
#include<string>
#include<time.h>

using namespace std;
long long listdir(struct node *,int depth, int max_depth);

/* validates the input path*/
int is_dir(char * dir)
{
   
   struct stat st;
   if (stat(dir, &st) != 0)
       return 0;
   if S_ISDIR(st.st_mode) return 1; //it is a directory
    else if S_ISREG(st.st_mode) return 2;// it is a file
    else return 0;

}

/* get the size of the given file (REG) in bytes*/
long int getsize(string path)
{
    struct stat st;
    // returns size if able to open, zero otherwise.
    if (stat(path.c_str(), &st) == 0)
    {
        return st.st_size;;
    }
    else
    {
        return 0;
    }

}

/* the tree node structure*/
struct node
{
    string name;
    long long int file_size=0;
    string dir;
    int numc=0; // num of children
    int depth=0;
   std:: vector<struct node *> children;// children vectors

};

/*traverse the tree built from parent root using depth first traversal*/
void TraverseTree (struct node *rt,FILE *pf)
{

    if(rt==NULL) return;

    for (int i=0;i< rt->numc;i++)
    {
        fprintf(pf,"%lld %s\n", rt->children[i]->file_size,(rt->children[i]->dir).c_str());//output size, dir in the file
        TraverseTree(rt->children[i],pf);
    }

}


int main()
{
    
    FILE *pf;
    pf = fopen("SysTree.txt", "w");//output file
    double start=clock();
    char parent[256]; //parent dir
    int max_depth;

    //input validation
    printf("Enter the parent Directory (just '/' to scan all hard disk):\n");
    scanf("%s", parent);
    printf("\n"); 

    int st = is_dir(parent);
    while(st==0)
    {
        printf("Wrong Path, Enter a correct Path: \n");
        scanf("%s", parent);
        st = is_dir(parent);
    }

    printf("Enter the max depth of the tree ( -1 to go to the full depth of the system):\n");
    scanf("%d", &max_depth);
    printf("\n"); 
    if(max_depth==-1) max_depth=INT_MAX;// to be able to go to the whole depth

    //create the root node
    struct node *rt=new node;   
    rt->dir= parent;
    long long int parent_dir_size;

    if(st==2) parent_dir_size=getsize(parent); // it is a file no need to do recursion or anything
    else if (st==1)
        parent_dir_size=listdir(rt,0, max_depth); // it is a dir, do recursion
    
    fprintf(pf,"%lld %s\n", rt->file_size,(rt->dir).c_str());
    printf("..................................................\n");
    
    printf("dir is %s, size is %lld \n", rt->dir.c_str(), parent_dir_size);// show the final output
    printf("..................................................\n");

    TraverseTree(rt,pf);
    fclose(pf);//close the file

    //calculate time taken
    double end = clock();
	double time = double(end - start) / CLOCKS_PER_SEC;

    printf("time taken = %f second\n",time);
    return 0;
}


/*builds the tress of all dirs and sub dirs recursively returning the size of the parent directory at the end */
long long listdir(struct node *rt, int depth, int max_depth)
{
    string path;
  
    DIR *dir = opendir((rt->dir).c_str());
  
  if(dir){
        struct dirent *dn;
    while ((dn = readdir(dir)) != NULL)
    {
       //( . means the current one,avoid reapeating the dir, inf recursion), also avoid LNK directories as they have huge depth, no size
        if (strcmp(dn->d_name, ".") != 0 && strcmp(dn->d_name, "..") != 0 && dn->d_type != DT_LNK)
        {

            // create new node 
            struct node* temp= new node;
         
	        path =(rt->dir)+"/"+string(dn->d_name);// new path
          
            temp->dir=path;
            
            rt->numc++;
            temp->name=dn->d_name;
           
            temp->file_size=getsize(path); //add file size in tree node
            rt->children.push_back(temp);//add to the children
            rt->depth=depth; //preserve the depth for frontend use
            
            if(dn->d_type==DT_REG)// it is a file, get its size
                {
                    rt->file_size +=getsize(path);
                
                }
                        
            else if(dn->d_type==DT_DIR) //it is a dir & depth is not max depth, do recursion
                {
                
                    if(depth< max_depth)
                        rt->file_size+=listdir(temp,depth+1,max_depth);
                }
        
        }
        
    }
   
    closedir(dir);
  }
    return rt->file_size;
}