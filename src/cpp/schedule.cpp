/*
志愿者排班 v1.0
遇到使用问题可以联系电气钱91张露承.

说明：
    - 分前八周排班和后八周排班；
    - 提供两种排班方式，1人/天(dfs_schedule_1)和2人/天(dfs_schedule_2)；
    - 单双周排班（两周轮换一次）使用2人/天排班即可；
    - 假设没有同名志愿者（如果有，在名字后加个数字区分就好了）；

TODO:
    - 用template元编程，写n人/天的排班法
    - 添加从excel读入和写出的方法(有·麻烦)
    - 用数据库存储报名结果，不用每次都重新录入（目前数据量比较少，录入的挺快；实现起来也有·麻烦）
    - 或许可以从读取txt做起，然后 excel -> txt -> c++ vector
*/

#include<cstdio>
#include<vector>
#include<string>
#include<cstring>
#include<set>
#include<ctime>

using namespace std;

class Volunteer{
public:
    string name;                                                                //姓名
    bool available_day_8[7];                                                    //前八周有空的时间
    bool available_day_16[7];                                                   //后八周有空的时间
    Volunteer(string Name, string Available_Day_8, string Available_Day_16);    //志愿者构造函数，可用的工作日以string传入，如 "12357"
};

Volunteer::Volunteer(string Name, string Available_Day_8, string Available_Day_16) : name(Name){   
    memset(available_day_8, false, 7);
    memset(available_day_16, false, 7);
    for(int i = 0; i < Available_Day_8.size(); ++i) 
        available_day_8[ Available_Day_8[i] - '1'] = true;
    for(int i = 0; i < Available_Day_16.size(); ++i) 
        available_day_16[ Available_Day_16[i] - '1'] = true;
}

//将报名的志愿者信息按日整理成表table，table8[i][j]表示前8周星期'i+1'报名的第j个志愿者
void makeTable(const vector<Volunteer> &workers, vector<vector<string>> &table8, vector<vector<string>> &table16){
    for(auto worker : workers){
        for(int i = 0; i < 7; i++){
            if(worker.available_day_8[i])
                table8[i].push_back(worker.name);
            if(worker.available_day_16[i])
                table16[i].push_back(worker.name);
        }
    }
}

//回溯法排班，每天排一个
void dfs_schedule_1(int i, const vector<vector<string>> &table, set<string> &check, 
                    vector<string> &item, vector<vector<string>> &res, bool &failed){
    if(i == 7) {
        res.push_back(item);
        return;
    }
    
    failed = true;      //failed表示是否这一天全部人都已经排过班
    for(int j = 0; j < table[i].size(); ++j){
        string workerName(table[i][j]);
        if ( check.find(workerName) == check.end() ){   //如果该志愿者此前未排班
            failed = false;
            check.insert(workerName);
            auto tmp_item = item;
            item.push_back(workerName);
            dfs_schedule_1(i+1, table, check, item, res, failed);
            check.erase(workerName);
            item = tmp_item;
        }
    }
    if(failed) return;  //如果这一天全部人都已经排过班，那就8行了
}

//回溯法排班，每天排两个
void dfs_schedule_2(int i, const vector<vector<string>> &table, set<string> &check, 
                    vector<string> &item, vector<vector<string>> &res, bool &failed){
    if(i == 7) {
        res.push_back(item);
        return;
    }
    
    failed = true;      //failed表示是否这一天全部人都已经排过班
    for(int j = 0; j < table[i].size()-1; ++j){
        string workerName1(table[i][j]);
        if ( check.find(workerName1) == check.end() ){
            check.insert(workerName1);
            auto tmp_item_1 = item;
            item.push_back(workerName1);
            for(int k = j+1; k < table[i].size(); ++k){
                string workerName2(table[i][k]);
                if(check.find(workerName2) == check.end()){
                    failed = false;
                    check.insert(workerName2);
                    auto tmp_item_2 = item;
                    item.push_back(workerName2);
                    dfs_schedule_2(i+1, table, check, item, res, failed);
                    check.erase(workerName2);
                    item = tmp_item_2;
                }
            }
            check.erase(workerName1);
            item = tmp_item_1;
        }
    }
    if(failed) return;  //如果这一天全部人都已经排过班，那就8行了
}

//输出排班结果，默认输出第一种情况
void display_schedule(vector<vector<string>> schedule, int i = 0){
    if(schedule.size() == 0){
        printf("8行，莫得结果\n");
        return;
    }
    printf("一共有%d种可能的情况，其中第%d种情况为：\n", schedule.size(), i+1);
    for(int j = 0; j < 7; ++j){
        if(schedule[i].size() == 7)
            printf("星期%d:\t%s\n", j+1, schedule[i][j].c_str());
        if(schedule[i].size() == 14)
            printf("星期%d:\t%s\t%s\n", j+1, schedule[i][2*j].c_str(), schedule[i][2*j+1].c_str());
    }
    return;
}

int main(){
    srand(unsigned(time(NULL)));
    vector<vector<string>> table8(7), table16(7), schedule8, schedule16;    //table用于存储报名结果，schedule存储排班结果
    vector<string> item8, item16;                                           //用于深搜暂存当前结果的一个容器
    set<string> check8, check16;                                            //用于存储当前已经排过的志愿者
    bool failed8 = false, failed16 = false;                                 //用于存储排班是否失败

    //输入志愿者报名信息，格式如下所示，第一个字符串是名字，第二个字符串表示前八周能工作的时间，第三个字符串是后八周能工作的时间
    vector<Volunteer> offline_volunteers = { 
        {"潘翔宇", "2", "12"}, {"田翰凌", "1567", "1245"}, {"赵耀宇", "67", "67"},
        {"柯春旭", "1245", "1245"}, {"王昊男", "12", "2"}, {"李甲辰", "25", "25"},
        {"刘健琛", "267", "267"}, {"王彦博", "35", "2345"}, {"冯博彦", "147", "147"},
        {"李龙飞", "367", "23567"}, {"夏徵羽", "1345", "12345"}, {"周明烁", "567", "567"},
        {"钞祎权", "124", "24"}, {"杨艾宸", "24", "24"},
    };
    vector<Volunteer> online_volunteers = { 
        {"李林怿", "67", "167"}, {"裴兆辰", "12457", "12457"}, {"陈依凡", "246", "136"}, 
        {"蔡子坚", "35", "35"}, {"刘博宁", "345", "345"}, {"陈斌龙", "67", "2567"},
        {"母彦琛", "2", "25"}, {"郭译文", "136", "136"},
    };

    makeTable(offline_volunteers, table8, table16);                         //线上排班第一个参数改成online..即可
    // dfs_schedule_2(0, table8, check8, item8, schedule8, failed8);           //前八周排班
    dfs_schedule_2(0, table16, check16, item16, schedule16, failed16);    //后八周排班

    //输出排班结果
    // printf("前8周");
    // display_schedule(schedule8, rand()%schedule8.size()); //当schedule8.size()==0时会出错，程序不报错也无输出
    printf("后8周");
    display_schedule(schedule16, rand()%(schedule16.size()));
    return 0;
}