////
//// Created by mihush on 11/12/2017.
////
//
//#ifndef DEDUPLICATION_PROJ_HASHTABLE_H
//#define DEDUPLICATION_PROJ_HASHTABLE_H
//
//#pragma once
//
//#include "AVL_t.h"
//
///*========================================Defines=====================================*/
//#define INITIAL_SIZE 4
//
//
///*============================== HashTable class ==================================*/
//template<class Key, class DataInfo, class HashFunc>
//class HashTable {
//    AVL_t<Key, DataInfo>** TreesArr;			//Array of pointers to AVL trees,
//    int ArrSize;									//Size of the Trees array
//    int NumOfElements;								//number of elements in the hash table
//
//    //Dynamic Array enums
//    enum SizeChange {
//        HalfSize, DoubleSize
//    };
//    /*=========================================Helper Methods===========================================*/
//
//    //Changes the tree array size, if DoubleSize flag is true, then doubles the size, if flag is false, then cut size by half
//    //also realloc the location of the data according to the new HashFunc (function changes by size of array)
//    void ChangeTreeArraySize(SizeChange size) {
//        int oldSize = this->ArrSize;
//        AVL_t< Key, DataInfo>** oldArray = this->TreesArr;
//        int newSize;
//        switch (size) {
//            case HalfSize: newSize = ArrSize / 2; break;
//            case DoubleSize: newSize = ArrSize * 2; break;
//            default: break;
//        }
//        CreateNewEmptyTreesArr(newSize);
//
//        //this for loop insert all oldSize elements from the hash table into the new hash table
//        //outer loop goes over the cells
//        //inner loop goes over all elements in a tree from a single cell
//        //total of oldSize insertions
//        for (int i = 0; i < oldSize; i++) {
//            if (oldArray[i] == NULL) {
//                continue;
//            }
//            int treeSize = ((AVL_t<Key, DataInfo>*)oldArray[i])->GetTreeSize();
//            DataInfo** dataInfoArray = new DataInfo*[treeSize];
//            Key* keysArray = new Key[treeSize];
//            ((AVL_t<Key, DataInfo>*)oldArray[i])->TreeToArrayWithKeys(keysArray, dataInfoArray);
//            for (int j = 0; j < treeSize; j++) {
//                AuxInsert(keysArray[j], dataInfoArray[j]);
//            }
//            delete[] keysArray;
//            delete[] dataInfoArray;
//            delete ((AVL_t<Key, DataInfo>*)oldArray[i]); //deletes the tree
//        }
//
//        delete[] oldArray;
//    }
//
//    //Creates a new empty Trees array in the class with the given size
//    void CreateNewEmptyTreesArr(int newSize) {
//        AVL_t< Key, DataInfo>** newArr = new AVL_t< Key, DataInfo>*[newSize];
//        for (int i = 0; i < newSize; i++) {
//            newArr[i] = NULL;
//        }
//        this->ArrSize = newSize;
//        this->TreesArr = newArr;
//    }
//
//    //Auxiliary insert function. insert the id and data to the tree in the array according to the hash function
//    void AuxInsert(const Key& key, DataInfo* info) {
//        int index = HashFunc()(key, ArrSize);
//        if (!TreesArr[index]) {	//if no tree in the index location
//            TreesArr[index] = new AVL_t<Key, DataInfo>();	//crate new tree
//        }
//        //now insert the new key to the tree
//        ((AVL_t<const Key, DataInfo>*)TreesArr[index])->Insert(key, info);
//    }
//
//
//public:
//    HashTable();
//    ~HashTable();
//    DataInfo* Find(const Key& key);
//    void Insert(const Key& key, DataInfo* data);
//    void Remove(const Key& key);
//    int GetNumOfHashElements();
//    void HashTable2Array(DataInfo** data_array);
//    /*================Debugging functions======================*/
//    int GetTableSize() {
//        return ArrSize;
//    }
//
//};
//
///*======================================== Implementation ==============================================*/
//
//
////Constructor
////Sets the array size to #define INITIAL_SIZE
//template<class Key, class DataInfo, class HashFunc>
//HashTable<Key, DataInfo, HashFunc>::HashTable() : TreesArr(new AVL_t<Key, DataInfo>*[INITIAL_SIZE]),
//                                                  ArrSize(INITIAL_SIZE), NumOfElements(0) {
//    for (int i = 0; i < INITIAL_SIZE; i++) {
//        TreesArr[i] = NULL;
//    }
//}
//
////Destructor - goes over the array and destroy every tree, in the end destroy the array iteself
//template<class Key, class DataInfo, class HashFunc>
//HashTable<Key, DataInfo, HashFunc>::~HashTable() {
//    for (int i = 0; i < ArrSize; i++) {
//        if (!this->TreesArr[i]) {
//            continue;
//        }
//        delete this->TreesArr[i];
//    }
//    delete[] TreesArr;
//}
//
////Find function. The function will find and return the info of the element with the given key
////if element with the given key wasnt found, will return NULL
//template<class Key, class DataInfo, class HashFunc>
//DataInfo * HashTable<Key, DataInfo, HashFunc>::Find(const Key& key) {
//    HashFunc hash_func;
//    int index = hash_func(key, ArrSize);
//    if (!TreesArr[index]) {
//        return NULL;
//    }
//    DataInfo* data = ((AVL_t< Key, DataInfo>*) TreesArr[index])->Find(key);
//    if (!data) {  //if the element wasnt found in the tree
//        return NULL;
//    }
//    return data;
//}
//
////inserts the data in the hash table according to the id and the hashfunction
////the data will be inserted in a AVL tree in each cell in the array
//template<class Key, class DataInfo, class HashFunc>
//void HashTable<Key, DataInfo, HashFunc>::Insert(const Key& key, DataInfo* data) {
//    if (Find(key)) {		//if the element was found in the hash table, we cant insert then return
//        return;
//    }
//    NumOfElements++;
//    if (NumOfElements >= ArrSize) {	//if we inserted more elements than the size of the array
//        ChangeTreeArraySize(DoubleSize);	//We use the dynmic array ability and double the size of the array
//    }
//    //uses the hash function inorder to get the key index, and then insert the id and data into the tree in that index
//    AuxInsert(key, data);
//    return;
//}
//
////deletes the data from the array according to the key given
////will delete from the avl tree in that cell array according to the index calculated from key with has function
//template<class Key, class DataInfo, class HashFunc>
//void HashTable<Key, DataInfo, HashFunc>::Remove(const Key& key) {
//    int index = HashFunc()(key, ArrSize);
//    if (TreesArr[index] == NULL) {
//        return;
//    }
//    ((AVL_t< Key, DataInfo>*) TreesArr[index])->Delete(key);
//    NumOfElements--;
//
//    //if the table is 0.25 full, we cut the array size by half and realloc the data
//    if (NumOfElements <= ArrSize / 4) {
//        ChangeTreeArraySize(HalfSize);
//    }
//    return;
//}
//
////returns the number of elements that are currently in the hash table
//template<class Key, class DataInfo, class HashFunc>
//int HashTable<Key, DataInfo, HashFunc>::GetNumOfHashElements() {
//    return NumOfElements;
//}
//
////Copy the entire hashtable data into a single data_array.
////the data is copied not in a specific order
////data_array should be pre allocated array of enough size (NumOfElements size)
//template<class Key, class DataInfo, class HashFunc>
//void HashTable<Key, DataInfo, HashFunc>::HashTable2Array(DataInfo** data_array) {
//    if (data_array == NULL) {
//        return;
//    }
//    //goes over the entire array and copies each tree
//    for (int i = 0; i < ArrSize; i++) {
//        //make sure there is a tree alocated in this cell
//        if (TreesArr[i] == NULL) {
//            continue;
//        }
//        int tree_size = ((AVL_t< Key, DataInfo>*) TreesArr[i])->GetTreeSize();
//        //if no info to copy from this tree, continue;
//        if (tree_size == 0) {
//            continue;
//        }
//        //copied the current tree into the current section of the array
//        ((AVL_t< Key, DataInfo>*) TreesArr[i])->TreeToArray(data_array);
//        //advances the data_array by the size of the tree.
//        data_array += tree_size; //
//    }
//    return;
//}
//
//#endif //DEDUPLICATION_PROJ_HASHTABLE_H
