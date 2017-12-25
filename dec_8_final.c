/*
* Change your function "main" to "xmain"
* Leave all of your project code intact. 
* Write a main to match the expected output:
I attest that the code submitted here is the
same code that I will submit in Canvas
and demonstrate to Dr. Shaffer
*/

// Have at it! Use the info from other assignments
// and from class to implement this one.

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#define MAXINPUTLENGTH 1000
#define MAXLENOFFIELDNAMES 50
#define MAXLENOFFIELDTYPES 50
#define MAXLENOFFIELDNUMBERS 10
#define MAXLENOFTABLENAME 50
#define MAXFIELDS 100
#define MAXTOKENLENGTH 50

struct _field {
   char fieldName[MAXLENOFFIELDNAMES];
   char fieldType[MAXLENOFFIELDTYPES];
   int fieldLength;
};
struct _table {
   char *tableFileName;
   int reclen;
   int fieldcount;
   struct _field fields[MAXFIELDS]; 
   int isSorted;
};

struct LinkedList{
   char data[MAXTOKENLENGTH];
   struct LinkedList *next;
};

typedef enum{false, true} bool;
typedef struct LinkedList *node;
typedef struct _table T;

char* toSchema(char* f){
   char *name = 0;
   if((name = (char *)malloc(strlen(f) + strlen(".schema") + 1)) != NULL){
      strcpy(name, f);
      strcat(name, ".schema");
   }
   
   f = malloc(sizeof(name));
   strcpy(f, name);
}

char* toBin(char* f){
   char *name = 0;
   if((name = (char *)malloc(strlen(f) + strlen(".bin") + 1)) != NULL){
      strcpy(name, f);
      strcat(name, ".bin");
   }
   
   f = malloc(sizeof(name));
   strcpy(f, name);
}


void copyfile(char *a, char *b){
   
   FILE *fptr1 = fopen(b, "r");
   FILE *fptr2 = fopen(a, "w");
   char c;
   c = fgetc(fptr1);
    while (c != EOF)
    {
        fputc(c, fptr2);
        c = fgetc(fptr1);
    }
   fclose(fptr1);
   fclose(fptr2);
}

void mixFile(char *d, char *a, char *b){
   FILE *fptr1 = fopen(d, "w");
   FILE *fptr2 = fopen(a, "r");
   FILE *fptr3 = fopen(b, "r");
   char c;
   c = fgetc(fptr2);
    while (c != EOF)
    {
        fputc(c, fptr1);
        c = fgetc(fptr2);
    }
   
   c = fgetc(fptr3);
    while (c != EOF)
    {
        fputc(c, fptr1);
        c = fgetc(fptr3);
    }
   fclose(fptr1);
   fclose(fptr2);
   fclose(fptr3);
}

void copyall(char *a, char *b){
   copyfile(toBin(a), toBin(b));
   copyfile(toSchema(a), toSchema(b));
}

node createNode(){
   node temp;
   temp = (node)malloc(sizeof(struct LinkedList));
   temp->next = NULL;
   return temp;
}

node addNode(node head, char *value){
   
   node temp, p;
   temp = createNode();
   strcpy(temp->data, value);
   
   if(head == NULL){
      head = temp;
   }else{
      p = head;
      while(p->next != NULL){
         p = p->next;
      }
      p->next = temp;
   }
   return head;
}

void trimwhitespace(char *buffer){
   for(int i= 0; i < strlen(buffer) ; i++) { 
      if (buffer[i] < ' '){
            buffer[i] = 0;
      }
      if(buffer[strlen(buffer) - 1] == ' '){
         buffer[strlen(buffer) - 1] = 0;    
      }
   } 
}

void dropExecute(char *buffer, char *filename){
   int i = 0;
   i = remove(toBin(filename));
   i = remove(toSchema(filename));
}

void loadSchema(T *table, char *filename){
   char *temp = (char *) malloc(MAXTOKENLENGTH);
   int margin = 0;
   int counter = 0;
   int counter2 = 0;
   int fieldLength = 0;
   int record_length = 0;

   FILE *f_schema = fopen(toSchema(filename), "rb");
   if (!f_schema){
      printf("Unable to open a file.");
   }
   fseek(f_schema, 0L, SEEK_END);
   int sz = ftell(f_schema);
   
   table->tableFileName = malloc(strlen(toBin(filename)));
   strcpy(table->tableFileName , toBin(filename));
   
   
   while (margin < sz){
      if (counter ==0){
         fseek(f_schema, margin, SEEK_SET);
         fread(temp, MAXLENOFFIELDNAMES, 1, f_schema);
         margin = margin + MAXLENOFFIELDNAMES;
         strcpy(table->fields[counter2].fieldName , temp);
      }else if (counter ==1){
         fseek(f_schema, margin, SEEK_SET);
         fread(temp, MAXLENOFFIELDTYPES, 1, f_schema);
         margin = margin + MAXLENOFFIELDTYPES;
         strcpy(table->fields[counter2].fieldType , temp);
      }else if (counter ==2){
         fseek(f_schema, margin, SEEK_SET);
         fread(temp, MAXLENOFFIELDNUMBERS, 1, f_schema);
         
         while( *temp < '0' || *temp > '9')
            ++temp;
         fieldLength = atoi(temp);
         
         record_length += fieldLength;
         margin = margin + MAXLENOFFIELDNUMBERS;
         
         table->fields[counter2].fieldLength = fieldLength;
         
         counter = -1;
         counter2++;
         
      }
      counter++;
   }
   
   table->reclen = record_length;
   table->fieldcount = counter2;
   fclose(f_schema);

}


void mergeControl(char *filename, char *key1, char *key2){
   T table;
   T table2;
   
   char *dest = "rewrite";
   char *temp_merge = "temp";
   char *buffer = malloc(MAXTOKENLENGTH);
   char *first = malloc(MAXTOKENLENGTH);
   char *second = malloc(MAXTOKENLENGTH);
   char *copy = malloc(MAXTOKENLENGTH);
   buffer = strtok(filename, ",");
   int count = 0;
   int checker = 0;
   
   while(buffer != NULL){
      
      if(count == 0){
         strcpy(first, buffer);
      }else if(count == 1){
         strcpy(second, buffer);
      }
      
      buffer = strtok(NULL, ",");
      count++;
   }
   
   copyall(temp_merge, first);
   
   loadSchema(&table, temp_merge);
   
   FILE *f_bin = fopen(toBin(temp_merge), "rb");
   fseek(f_bin, 0, SEEK_END);
   int sz = ftell(f_bin);
   int base = 0;
   int index = 0;
   
   FILE *mergeFile = fopen(toBin(dest), "wb");
   
   while (base < sz){
      index = base;
      for(int i = 0; i < (table.fieldcount); i ++){
         if(strcmp(table.fields[i].fieldName , key1) == 0){
            
            fseek(f_bin, index, SEEK_SET);
            char *output = malloc(table.fields[i].fieldLength);
            fread(output , table.fields[i].fieldLength, 1, f_bin);
            loadSchema(&table2, second);
            FILE *f2_bin = fopen(toBin(second), "rb");
            fseek(f2_bin, 0, SEEK_END);
            int sz2 = ftell(f2_bin);
            int base2 = 0;
            int index2 = 0;
            
            while(base2 < sz2){
               index2 = base2;
               
               for(int j = 0; j < (table2.fieldcount); j ++){
                  if(strcmp(table2.fields[j].fieldName , key2) == 0){
                     
                     fseek(f2_bin, index2, SEEK_SET);
                     char *output2 = malloc(table2.fields[j].fieldLength);
                     fread(output2 , table2.fields[j].fieldLength, 1, f2_bin);
                     
                     if (strcmp(output , output2) == 0){
                        checker = 1;
                        mixFile(toSchema(dest), toSchema(temp_merge), toSchema(second));
                        
                        int margin = base;
                        
                        for(int k =0; k < table.fieldcount ; k++){
                           char *a = malloc(MAXINPUTLENGTH);
                           fseek(f_bin, margin, SEEK_SET);
                           fread(a , table.fields[k].fieldLength, 1, f_bin);
                           fwrite(a, sizeof(char) * table.fields[k].fieldLength, 1, mergeFile);
                           margin += table.fields[k].fieldLength;
                        }
                        
                        int margin2 = base2;
                        for(int k =0; k < table2.fieldcount ; k++){
                           char *b = malloc(MAXINPUTLENGTH);
                           fseek(f2_bin, margin2, SEEK_SET);
                           fread(b , table2.fields[k].fieldLength, 1, f2_bin);
                           fwrite(b, sizeof(char) * table2.fields[k].fieldLength, 1, mergeFile);
                           margin2 += table2.fields[k].fieldLength;
                        }
                     }
                     //base = sz;
                     //break;
                  }
                  index2 = index2 + table2.fields[j].fieldLength;
               }
               base2 += table2.reclen;
            }
            fclose(f2_bin);
         }
         index = index + table.fields[i].fieldLength;
      }
      
      base += table.reclen;
   } 
   fclose(mergeFile);
   fclose(f_bin);
}


char* premerge(node tables, int flag){
   char *dest = "rewrite";
   char *merge = malloc(MAXTOKENLENGTH);
   int counter = 0;
   FILE *err = fopen("error.txt", "a");
   fprintf(err, "tables is %s and flag is %d\n", tables->data, flag);
   fclose(err);
   while(tables!=NULL){
      if (flag ==0){
         strcpy(merge, tables->data);
         strcat(merge, ",");
         strcat(merge, tables->next->data);
         break;
      }else{
         strcpy(merge, dest);
         if(counter == flag + 1){
            strcat(merge, ",");
            strcat(merge, tables->data);
            break;
         }
      }
      tables=tables->next;
      counter++;
   }
   return merge;
}


void display(char *copy, node files, int toFile){
   T table;
   char *token = malloc(MAXTOKENLENGTH);
   char *output = malloc(MAXTOKENLENGTH);
   char *outputs = malloc(MAXINPUTLENGTH);
   char *log = malloc (MAXINPUTLENGTH);
   int index = 0;
   int base = 0;
   int count = 0;
   char *index_F = "indexed";
   
   loadSchema(&table, files->data);
  
   
   FILE *f_bin = fopen(toBin(files->data), "rb");
   fseek(f_bin, 0, SEEK_END);
   int sz = ftell(f_bin);
   
   
   FILE *indexFile = fopen(toBin(index_F),"wb");
   FILE *indexSchema = fopen(toSchema(index_F),"wb");
   
   
   while (base < sz){
      log = malloc (MAXINPUTLENGTH);
      index = base;
      strcpy(outputs, copy);
      token = strtok(outputs, ",");
      int cheat = 0;
      while (token != NULL){
         for (int i = 0; i < (table.fieldcount); i ++){
            if(strcmp(table.fields[i].fieldName , token) == 0){
              
               fseek(f_bin, index, SEEK_SET);
               output = malloc(table.fields[i].fieldLength);
               fread(output , table.fields[i].fieldLength, 1, f_bin);
               
               if(toFile == 0){
                  if(count == 0){
                     char *t = malloc(MAXLENOFFIELDNUMBERS);
                     sprintf(t, "%d", table.fields[i].fieldLength);
                     
                     fwrite(table.fields[i].fieldName, MAXLENOFFIELDNAMES, 1, indexSchema);
                     fwrite(table.fields[i].fieldType , MAXLENOFFIELDTYPES, 1, indexSchema);
                     fwrite(t, MAXLENOFFIELDNUMBERS, 1, indexSchema);
                  }
                  fwrite(output, sizeof(char) * table.fields[i].fieldLength, 1, indexFile);
               }else{
                  if(cheat == 0){
                     strcpy(log, output);
                     cheat = 1;
                  }else{
                     strcat(log, ",");
                     strcat(log, output);
                  }
               }
            } 
            index = index + table.fields[i].fieldLength;
         }
         index = base;
         token = strtok(NULL, ",");
      }
      if(toFile != 0){
         printf("%s\n", log);
      }
      base += table.reclen;
      count++;
   }
   fclose(indexFile);
   fclose(indexSchema);
   fclose(f_bin);
}


bool checkSorted(char* filename){
   //printf("in check phase %s\n", filename);
   char* SortedList = "SortedFile.txt";
   FILE* checkSort = fopen(SortedList, "r");
   char* SortedFiles = malloc(MAXTOKENLENGTH);
   int index = 0;
   
   fseek(checkSort, 0, SEEK_END);
   int sz = ftell(checkSort);
   
   while(index < sz){
      fseek(checkSort, index, SEEK_SET);
      fread(SortedFiles, MAXTOKENLENGTH, 1, checkSort);
      if (strcmp(SortedFiles, filename) == 0){
         //printf("in check phase2 %s\n", filename);
         return 1;
      }
      
      index += MAXTOKENLENGTH;
   }
   fclose(checkSort);
   return 0;
   
}

void b_search(int min, int size, T table, char* field, char* value, char* filename){
   char* dest = "rewrite";
   int middle = min + size/2;
   node p_data = NULL;
   char* log = malloc(500);
   char* output = malloc(MAXTOKENLENGTH);
   int base =0;
   int index = 0;
   int flag = 0;
   FILE* f = fopen(toBin(filename), "r");
   
   for (int i = 0; i < table.fieldcount; i++){
      if(strcmp(table.fields[i].fieldName, field) == 0){
         index = i;
         base = flag;
      }
      fseek(f, middle * table.reclen + flag, SEEK_SET);
      fread(log, table.fields[i].fieldLength, 1, f);
      p_data = addNode(p_data, log);
      flag += table.fields[i].fieldLength;
   }
   
   printf("TRACE: ");
   while(p_data != NULL){
      printf("%s", p_data->data);
      if(p_data->next != NULL){
         printf(",");
      }else{
         printf("\n");
      }
      p_data = p_data->next;
   }

   fseek(f, middle * table.reclen + base, SEEK_SET);
   fread(output, table.fields[index].fieldLength, 1, f);
   flag = 0;
   if (strcmp(output, value)== 0){
      FILE *f_write = fopen(toBin(dest), "w");
      for (int i = 0; i < table.fieldcount; i++){
         fseek(f, middle * table.reclen + flag, SEEK_SET);
         fread(log, table.fields[i].fieldLength, 1, f);
         fwrite(log, table.fields[i].fieldLength, 1, f_write);
         flag += table.fields[i].fieldLength;
      }
      copyfile(toSchema(dest), toSchema(filename));
      fclose(f_write);
      return;
   }else if (strcmp(output, value) < 0){
      return(b_search(middle, size-middle ,table ,field, value, filename));
   }else{
      return(b_search(min, middle - min ,table ,field, value, filename));
   }
}

int getRows(char* filename, T table){
   FILE* f = fopen(toBin(filename), "r");
   fseek(f, 0, SEEK_END);
   int sz = ftell(f);
   fclose(f);
   return sz / table.reclen;
}

void processwhere(node tables, node where){
   T table;
   char *temp = "temp";
   char *dest = "rewrite";
   char *output = malloc(MAXINPUTLENGTH);
   char *log = malloc (MAXINPUTLENGTH);
   
   int index = 0;
   int base = 0;
   int counter = 0;
   int counter2 = 0;
    int equi = 0;
  
   
   while(where!=NULL){
      char *token1 = where->data;
      where = where->next;
      char *equality_token = where->data;
      where = where->next;
      char *token2 = where->data;
      where = where->next;
      
      
      if(token2[strlen(token2) - 1] == '"'){
         token2[strlen(token2) - 1] = 0;  

         memmove(&token2[0], &token2[1], strlen(token2) - 0); 
          
         
         if(counter == 0){
            copyall(temp, tables->data);
         }
         FILE *err = fopen("error.txt", "a");
   fprintf(err, "in processwhere: token1 is %s and token2 is %s next is %s and table is %s\n", token1, token2, where->data, tables->data);
   fclose(err);
               
              

         counter++;
         loadSchema(&table, temp);
         
        
         if(checkSorted(tables->data)){
            b_search(0, getRows(tables->data, table), table, token1, token2, tables->data);
            return;
         }
        
         
         FILE *f_bin = fopen(toBin(temp), "r");
         FILE *rewrite = fopen(toBin(dest), "wb");
   
         fseek(f_bin, 0, SEEK_END);
         int sz = ftell(f_bin);
         base = 0;
   
         while (base < sz){
     
            log =malloc (MAXINPUTLENGTH);
            index = base;
            int checker = 0;
           
      
            for (int i = 0; i < (table.fieldcount); i ++){
               if(strcmp(table.fields[i].fieldName , token1) == 0){
                  fseek(f_bin, index, SEEK_SET);
                  output = malloc(table.fields[i].fieldLength);
                  fread(output , table.fields[i].fieldLength, 1, f_bin);
                  //printf("STRING COMPARISON: %d equality token is : %d\n", strcmp(token2, output), strcmp(equality_token, ">"));
                  if((strcmp(equality_token, "=") == 0)&&(strcmp(token2, output) == 0)){
                     
                     int margin = base;
                     for(int j =0; j < table.fieldcount ; j++){
                  
                        char *a = malloc(MAXINPUTLENGTH);
                        fseek(f_bin, margin, SEEK_SET);
                        fread(a , table.fields[j].fieldLength, 1, f_bin);
                        
                        fwrite(a, sizeof(char) * table.fields[j].fieldLength, 1, rewrite);
                        margin += table.fields[j].fieldLength;
                        
                     }
                     
                  }else if((strcmp(equality_token, "<") == 0)&&(strcmp(token2, output) > 0)){
                     int margin = base;
                     for(int j =0; j < table.fieldcount ; j++){
                  
                        char *a = malloc(MAXINPUTLENGTH);
                        fseek(f_bin, margin, SEEK_SET);
                        fread(a , table.fields[j].fieldLength, 1, f_bin);
                        if(equi == 2)
                           equi = 0;
                        if (strcmp(token2, a) > 0 || equi == 1){
                           fwrite(a, sizeof(char) * table.fields[j].fieldLength, 1, rewrite);
                        }else{
                              base = sz;
                              break;
                        }
                        margin += table.fields[j].fieldLength;
                        equi++;
                        
                        //equi++;
                        
                     }
                  }else if((strcmp(equality_token, ">") == 0)&&(strcmp(token2, output) < 0)){
                     int margin = base;
                     for(int j =0; j < table.fieldcount ; j++){
                  
                        char *a = malloc(MAXINPUTLENGTH);
                        fseek(f_bin, margin, SEEK_SET);
                        fread(a , table.fields[j].fieldLength, 1, f_bin);
                        fwrite(a, sizeof(char) * table.fields[j].fieldLength, 1, rewrite);
                        margin += table.fields[j].fieldLength;
                        
                     }
                  }
               }
               index = index + table.fields[i].fieldLength;
            }
            base += table.reclen;
         }
   
         copyfile(toSchema(dest), toSchema(temp));
         fclose(rewrite);
         fclose(f_bin);
      
         copyall(temp, dest);
   
      }else{
         mergeControl(premerge(tables, counter2), token1, token2);
         copyall(temp, dest);
         //return;
         counter2++;
         counter++;
      }
      
   }
}


void loadExecute(char *buffer, char *filename){
   T table;
   char *token = malloc(MAXINPUTLENGTH);
   char *small_token = malloc(MAXLENOFTABLENAME);
   strcpy(token ,buffer);
   
   loadSchema(&table, filename);
   
   FILE* f = fopen(toBin(filename), "a");
   trimwhitespace(token);
   
   small_token = strtok(token, ",");
   int len = 0;
   int counter = 0;
   while(small_token != NULL){
      trimwhitespace(small_token);
      len = table.fields[counter].fieldLength;
      
      if (counter == table.fieldcount - 1){
         counter = 0;
      }else{
         counter++;
      }
         
      char token2[len];
      if(strlen(small_token) >= len -1){
         printf("*** WARNING: Data in field %s is being truncated ***\n", table.fields[counter].fieldName);
      }   
      strncpy(token2, small_token, len-1);
         
      strcat(token2, "\0");
      fwrite(token2, sizeof(char) * len, 1, f);
      small_token = strtok(NULL, ",");
   }
   fclose(f);
   return;
}



void createExecute(char *buffer, char *filename, FILE* f_in){
   int counter;
   char *token = malloc(MAXTOKENLENGTH);
   char *small_token = malloc(MAXLENOFTABLENAME);
   dropExecute(buffer, filename);
   
   token = fgets(buffer, MAXINPUTLENGTH-1, f_in);
   
   FILE *f = fopen(toSchema(filename), "wb");
   if (!f){
      printf("Unable to open a file.");
   }
   while (token != NULL){
      trimwhitespace(token);
      printf("===> %s\n", token);
      if (strcmp(token , "END") == 0){
         fclose(f);
         return;
      }
      counter = 0;
      small_token = strtok(token, " ");
      while(small_token != NULL){
         
         if (counter == 0){
         }else if (counter == 1){
            printf("SMALL TOKENS: %s\n", small_token);
            fwrite (small_token, MAXLENOFFIELDNAMES, 1, f);
         }else if (counter == 2){
            printf("SMALL TOKENS: %s\n", small_token);
            fwrite (small_token, MAXLENOFFIELDTYPES, 1, f);
         }else if(counter == 3){
            printf("SMALL TOKENS: %s\n", small_token);
            fwrite (small_token, MAXLENOFFIELDNUMBERS, 1, f);
         }
         
         small_token = strtok(NULL, " ");
         counter++;
      }
      token = fgets(buffer, MAXINPUTLENGTH-1, f_in);
   }
   fclose(f);
}


void selectExecute(char *buffer, FILE* f_in){
   
   char *token = malloc(MAXINPUTLENGTH);
   char *copy = malloc(MAXTOKENLENGTH);
   char *copy2 = malloc(MAXTOKENLENGTH);
   char *outputs = malloc(MAXTOKENLENGTH);
   char *table_limit = malloc(MAXTOKENLENGTH);
   char *dest = "rewrite";
   int count = 0;
   int checker = 0;
   
   token = buffer;
   node p = NULL;
   node p_tables = NULL;
   
   while (token != NULL){
      trimwhitespace(token);
      printf("===> %s\n", token);
      copy = strtok(token, " ");
      strcpy(copy2, copy);
      count = 0;
      
      if(strcmp(copy2 , "SELECT") == 0){
         while(copy != NULL){
            if (count == 0){
            }else if(count == 1){
               strcpy(outputs,copy);
            }else{
               strcat(outputs,copy);
            }
            copy = strtok(NULL ," ");
            count++;
         }
      }
      
      if(strcmp(copy2 , "FROM") == 0){
         while(copy != NULL){
            if (count != 0){
               p_tables = addNode(p_tables, copy);
               if(count == 1){
                  strcpy(table_limit, copy);
               }else{
                  strcpy(table_limit, dest);
               }
            }
            copy = strtok(NULL ," ");
            count++;
         }
      }
      
      if (strcmp(copy2 , "END") == 0){
         break;
      }
      
      if((strcmp(copy2 , "WHERE") == 0)||(strcmp(copy2 , "AND") == 0)){
         checker = 1;
         while(copy != NULL){
            if (count != 0){
               p = addNode(p, copy);
            }
            copy = strtok(NULL ," ");
            count++;
         }
      }
      token = fgets(buffer, MAXINPUTLENGTH-1, f_in);
   }
   
   FILE *isEmpty  = fopen(toBin(table_limit), "rb");
   if (!isEmpty){
         printf("Table %s does not exist.\n", p_tables->data);
   }else{
      
      processwhere(p_tables, p);
      if(checker != 0){
         p_tables = NULL;
         p_tables = addNode(p_tables, dest);
      }
      display(outputs, p_tables, 1);
   }
   return;  
}

int toTextFile(char *dest, char *src){
   //return fieldcount
   int base = 0;
   int sz = 0;
   int index = 0;
   T table;
   FILE *inputs = fopen(toBin(src), "r");
   FILE *outputs = fopen(dest, "w+");
   char *log = malloc(100);
   
   loadSchema(&table, src);
   
   fseek(inputs, 0, SEEK_END);
   sz = ftell(inputs);
   base = 0;
   
   while (base < sz){
      index = base;
      
      for (int i = 0; i < (table.fieldcount); i ++){
         fseek(inputs, index, SEEK_SET);
         fread(log, table.fields[i].fieldLength, 1, inputs);
         if ( i == table.fieldcount - 1){
            fprintf (outputs, "%-50s\n", log);
         }else{
            fprintf (outputs, "%-50s,", log);
         }
         index = index + table.fields[i].fieldLength;
      }
      base += table.reclen;
   }
   fclose(inputs);
   fclose(outputs);
   return table.fieldcount;
}

void indexing(char *filename, char* field, node p_source){
   
   char* TextFile = "to_textTemp.txt";
   char* SortFile = "sort.srt";
   char* SortedList = "SortedFile.txt";
   int count = 0;
   int base = 0;
   int sz = 0;
   
   display(field, p_source, 0);
   copyall(filename, "indexed");
   int fieldcount = toTextFile(TextFile, "indexed");
   int fieldSize = fieldcount * 51;
   
   char* input = malloc(fieldSize);
   char* load = malloc(fieldSize);
   
   remove(toBin(filename));
   system("sort to_textTemp.txt > sort.srt");
   
   FILE *sorted = fopen(SortFile, "r");
   char *c = malloc(fieldSize);
     
   fseek(sorted, 0, SEEK_END);
   sz = ftell(sorted);
   base = 0;
   
   while (base < sz - 1){
      fseek(sorted, base, SEEK_SET);
      fread(c, fieldSize -1, 1, sorted);
      input = strtok(c, " ");
      while(input != NULL){
         if (count == 0){
            strcpy(load, input);
         }else if (count == fieldcount-1){
            strcat(load, input);
            loadExecute(load, filename);
            count = -1;
         }else{
            strcat(load, input);
         }
         count++;
         input = strtok(NULL, " ");
      }
      base += fieldSize;
   }
   fclose(sorted);
   
   FILE *SortedFile = fopen(SortedList, "r");
   fseek(SortedFile, 0, SEEK_END);
   if (ftell(SortedFile) != 0)
      remove(SortedList);
   fclose(SortedFile);
   
   SortedFile = fopen(SortedList, "a");
   fwrite(filename, MAXTOKENLENGTH, 1, SortedFile);
   fclose(SortedFile);
}


void indexExecute(char *buffer, FILE* f_in){
   char *token = malloc(MAXINPUTLENGTH);
   char *copy = malloc(MAXTOKENLENGTH);
   char *copy2 = malloc(MAXTOKENLENGTH);
   char *filename = malloc(MAXTOKENLENGTH);
   char *outputs = malloc(MAXTOKENLENGTH);
   char *dest = "rewrite";
   int count = 0;
   int checker = 0;
   int flag = 0;
   
   token = buffer;
   node p_field = NULL;
   node p_source = NULL;
   
   while (token != NULL){
      trimwhitespace(token);
      printf("===> %s\n", token);
      copy = strtok(token, " ");
      strcpy(copy2, copy);
      count = 0;
      
      if(strcmp(copy2 , "CREATE") == 0){
         while(copy != NULL){
            if (flag ==1){
               if (checker == 0){
                  strcpy(outputs,copy);
               }else{
                  strcat(outputs,copy);
               }
               checker++;
            }
            if (strcmp(copy, "USING") ==0){
               flag = 1;
            }
            if (count ==2){
               strcat(filename,copy);
            }
            copy = strtok(NULL ," ");
            count++;
         }
      }
      
      if(strcmp(copy2 , "FROM") == 0){
         while(copy != NULL){
            if (count != 0){
               p_source = addNode(p_source, copy);
            }
            copy = strtok(NULL ," ");
            count++;
         }
      }
      if (strcmp(copy2 , "END") == 0){
         break;
      }
      token = fgets(buffer, MAXINPUTLENGTH-1, f_in);
   }
   indexing(filename, outputs, p_source);
  
   return;
}


void processCommand(char *buffer, FILE* f_in){
   char *token = malloc(MAXINPUTLENGTH);
   char *small_token = malloc(MAXTOKENLENGTH);
   char *temp = malloc(MAXINPUTLENGTH);
   char *filename = malloc(MAXTOKENLENGTH);
   
   char *drop = "DROP";
   char *load = "INSERT";
   char *create = "CREATE";
   char *select = "SELECT";
   
   int flag = 0;
   int counter = 0;
   
   strcpy(temp, buffer);
   small_token = strtok(buffer, " ");
   
   if(strcmp(small_token, drop) == 0){
      printf("===> %s\n", temp);
      while(small_token != NULL){
         if(counter == 2){
            strcpy (filename, small_token);
         }
         counter++;
         small_token = strtok(NULL, " ");
      }
      dropExecute(temp, filename);
      flag = 0;
      return;
   }
   
   if(strcmp(small_token, load) == 0){
      char *input = malloc(strlen(temp));
      printf("===> %s\n", temp);
      while(small_token != NULL){
         if(counter == 2){
            strcpy (filename, small_token);
         }else if(counter == 3){
            strcpy (input, small_token);
         }else if(counter > 3){
            strcat (input, " ");
            strcat (input, small_token);
         }
         counter++;
         small_token = strtok(NULL, " ");
      }
      loadExecute(input, filename);
      return;
   }
   
   if(strcmp(small_token, create) == 0){
      int flag =0;
      //printf("===> %s\n", temp);
      while(small_token != NULL){
         if(counter == 1){
            if(strcmp(small_token, "INDEX") == 0){
               indexExecute(temp, f_in);
               return;
            }
         }
         if(counter == 2){
            
            strcpy (filename, small_token);
            break;
         }
         counter++;
         small_token = strtok(NULL, " ");
      }
      //fgets(temp, 
      printf("===> %s with FILENAME: %s\n", temp, filename);
      createExecute(temp, filename, f_in);
      
      return;
   }
   
   if(strcmp(small_token, select) == 0){
      selectExecute(temp, f_in);
      return;
   }
}

int main() {
   static char buffer[MAXINPUTLENGTH];
   memset(buffer, 0, MAXINPUTLENGTH);
   FILE* f = fopen("SortedFile.txt", "w");
   fclose(f);
   printf("Welcome!\n");
   FILE* f_in = fopen("setup.txt", "r");
   char *status = fgets(buffer, MAXINPUTLENGTH-1, f_in); 
   while (status != NULL) {
      trimwhitespace(buffer);
      if (strlen(buffer) < 5)
            break; // not a real command, CR/LF, extra line, etc.
      processCommand(buffer, f_in);
      status = fgets(buffer, MAXINPUTLENGTH-1, f_in);
    }
   fclose(f_in);
   printf("Goodbye!\n");
   return 0;
}