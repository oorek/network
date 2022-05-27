#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <utility> 
#include <vector>
#include <string>
using namespace std;
typedef pair<int, int> ii;
#define MAX_VAL 101
#define MAX_COST 100000000
FILE* input_file, * message_file, *change_file;
FILE* output_file;
class Table;
class graph;

class Table {
public:
	int T[100][2];
	int **path_vector;
	
	Table() {
		for (int i = 0; i < 100; i++) {
			T[i][0] = MAX_VAL;
			T[i][1] = MAX_COST;
		}
		path_vector = (int**)malloc(sizeof(int*) * 100);
		for (int i = 0; i < 100; i++)
			path_vector[i] = (int*)malloc(sizeof(int) * 101);
		for (int i = 0; i < 100; i++) {
			for (int j = 0; j < 101; j++) {
				path_vector[i][j] = MAX_VAL;
			}
		}
	}
	~Table() {
		for (int i = 0; i < 100; i++)
			free(path_vector[i]);
		free(path_vector);
	}
	void make_table(int next, int val) {
		T[next][0] = next;
		T[next][1] = val; //distance
	}
	int exchange_table(Table &tmp, int sender, int receiver, int flag) {
		for (int i = 0; i < 100; i++) {
			if (i == sender || i == receiver)
				continue;
			if (T[i][1] != MAX_COST) { // 업데이트 할거 줄게
				if (tmp.T[i][1] == MAX_COST) { // 거리 정보가 없다면 값 그대로 물려받기
					flag = 1;
					tmp.T[i][0] = sender;
					tmp.T[i][1] = T[i][1] + T[receiver][1];
				}
				else { // 있으면 거리 정보 비교해서 작은거
					if (tmp.T[i][1] >= T[i][1] + T[receiver][1]) { 
						if (tmp.T[i][1] == T[i][1] + T[receiver][1]) {
							if (T[receiver][0] == receiver) {
								int v = tmp.T[i][0];
								if (tmp.T[v][0] != v) { // 현재 receiver의 i로가는 next가 i가 아닐때 
									flag = 1;
									tmp.T[i][0] = sender;
								}
								else {
									if (tmp.T[i][0] > sender) {
										tmp.T[i][0] = sender;
									}
								}
							}
						}
						else {
							// sender의 최종 sender가 최솟값이 되어야함
							flag = 1;
							tmp.T[i][1] = T[i][1] + T[receiver][1];
							tmp.T[i][0] = sender;
						}
					}

				}
			}
		}

		if (!flag) {
		//	printf("unchanged sender : %d receiver: %d\n", sender, receiver);
			return 1;
		}
		else
			return 0;
	}
	void print_table(int node_number) {
		for (int i = 0; i < node_number; i++) {
			if (T[i][0] == MAX_VAL)
				continue;
			printf("%d %d %d\n", i, T[i][0], T[i][1]);
		}
	}
	void print_path_vector() {
		for (int i = 0; i < 5; i++) {
			for (int j = 0; j < 5; j++) {
				//if (path_vector[i][j] == MAX_VAL)
				//	break;
				printf("%d ", path_vector[i][j]);
			}
			printf("\n");
		}
	}
	void change_table(int next, int cost) {
		if (cost == MAX_COST) {
			T[next][0] = MAX_VAL;
			T[next][1] = MAX_COST;
		}
		else {
			T[next][0] = next;
			T[next][1] = cost;
		}
	}
	void make_path_vector(int j, int* tracking) {
		for (int k = 0; k < 100; k++) {
			if (tracking[k] == MAX_VAL) {
				path_vector[j][k] = MAX_VAL;
				break;
			}
			path_vector[j][k] = tracking[k];
		}
	}
	void update_vector(vector<pair<ii, int>> &p, int val, int sender, int receiver) {
		if (val == sender) {
			T[receiver][0] = MAX_VAL;
			T[receiver][1] = MAX_COST;
		}
		else if(val == receiver) {
			T[sender][0] = MAX_VAL;
			T[sender][1] = MAX_COST;
		}
		for (int i = 0; i < 100; i++) {
			for (int j = 0; j < 100; j++) {
				if (path_vector[i][j] == sender && path_vector[i][j + 1] == receiver) {
					T[i][0] = MAX_VAL;
					T[i][1] = MAX_COST;
					for (int m = 0; m < p.size(); m++) {
						if (p[m].first.first == val && p[m].first.second == i) {
							T[i][0] = i;
							T[i][1] = p[m].second;
							break;
						}
					}

					for (int k = 0; k < 101; k++)
						path_vector[i][k] = MAX_VAL;
					break;
				}
				else if (path_vector[i][j] == receiver && path_vector[i][j + 1] == sender) {
					T[i][0] = MAX_VAL;
					T[i][1] = MAX_COST;
					for (int m = 0; m < p.size(); m++) {
						if (p[m].first.first == val && p[m].first.second == i) {
							T[i][0] = i;
							T[i][1] = p[m].second;
							break;
						}
					}
					for (int k = 0; k < 101; k++)
						path_vector[i][k] = MAX_VAL;
					break;
				}
			}
		}
	}
	int ret_next(int end) {
		return T[end][0];
	}
	int ret_cost(int end) {
		return T[end][1];
	}
};

class graph {
public:
	int node_number;
	vector<pair<ii, int>> p;
	Table routing_table[100];

	graph() {
	}
	void initialize(int first, int sec, int value) {
		p.push_back(make_pair(make_pair(first, sec), value));
		p.push_back(make_pair(make_pair(sec, first), value));
	}
	void make_table(int node_num) {
		for (int i = 0; i < p.size(); i++) {
			if (p[i].first.first == node_num) {
				//초기 table 만들기
				routing_table[node_num].make_table(p[i].first.second, p[i].second);
			}
		 }
	}
	void make_all_table() {
		for (int i = 0; i < p.size(); i++) {
			routing_table[p[i].first.first].make_table(p[i].first.second, p[i].second);
			routing_table[p[i].first.first].make_table(p[i].first.first, 0);
		}
	}

	void exchange_table() {
		while (1) {
			int cnt = 0;
			for (int i = 0; i < p.size(); i++) {
				//int root[100];
				//find_root(root, p[i].first.first);
				cnt += routing_table[p[i].first.first].exchange_table(routing_table[p[i].first.second], p[i].first.first, p[i].first.second, 0);
			}
			//printf("cnt: %d %d\n", cnt, p.size());
			if (cnt == p.size())
				break;
		}
	}
	void print_table() {
		for (int i = 0; i < node_number; i++) {
			routing_table[i].print_table(node_number);
			printf("\n");
		}
	}
	void print_path_vector() {
		for (int i = 0; i < node_number; i++) {
			routing_table[i].print_path_vector();
			printf("\n");
		}
	}
	void make_path_vector() {
		for (int i = 0; i < 100; i++) {
			for (int j = 0; j < 100; j++) {
				int* tracking;
				tracking = (int*)malloc(sizeof(int) * 101);
				for (int m = 0; m < 101; m++)
					tracking[m] = MAX_VAL;

				int k = 0;
				int tmp = i;
				if (routing_table[i].ret_next(j) == MAX_VAL)
					continue;
				while (tmp != j) {
					tracking[k++] = tmp;
				//	printf("tmp: %d\n", tmp);
					tmp = routing_table[tmp].ret_next(j);
				//	printf("k : %d\n", k);
				}
				tracking[k] = tmp;
			//	printf("k : %d\n", k);
			//	routing_table[i].path_vector[j] = tracking;
				routing_table[i].make_path_vector(j, tracking);

				free(tracking);;
			}
		}
	}
	int track_path(int *tracking, int start, int end) {
		int k = 0;
		int tmp = start;
		if (routing_table[start].ret_next(end) == MAX_VAL)
			return  -1;
		while (tmp != end) {
			tracking[k++] = tmp;
			tmp = routing_table[tmp].ret_next(end);
		}
		int cost = routing_table[start].ret_cost(end);
		return cost;
	}
	void change_table(int start, int end, int cost) {
		if (cost == -999) {
			int flag = 0;
			for (int i = 0; i < p.size(); i++) {
				if (p[i].first.first == start && p[i].first.second == end) {
					if (i == 0 || i == 1) {
						p.erase(p.begin());
						p.erase(p.begin());
					}
					else if (i == p.size() - 1 || i == p.size() - 2) {
						p.erase(p.begin() + p.size() - 1);
						p.erase(p.begin() + p.size() - 1);
					}
					else {
						if (i % 2 == 0) {
							p.erase(p.begin() + i);
							p.erase(p.begin() + i);
						}
						else {
							p.erase(p.begin() + i - 1);
							p.erase(p.begin() + i - 1);
						}
					}
					break;
				}
			}
			routing_table[start].change_table(end, MAX_COST);
			routing_table[end].change_table(start, MAX_COST);
			return;
		}
		routing_table[start].change_table(end, cost);
		routing_table[end].change_table(start, cost);
		int flag = 0;
		for (int i = 0; i < p.size(); i++) {
			if (p[i].first.first == start && p[i].first.second == end) {
				flag = 1;
				if (i == 0 || i == 1) {
					p.erase(p.begin());
					p.erase(p.begin());
				}
				else if (i == p.size() - 1 || i == p.size() - 2) {
					p.erase(p.begin() + p.size() - 1);
					p.erase(p.begin() + p.size() - 1);
				}
				else {
					if (i % 2 == 0) {
						p.erase(p.begin() + i);
						p.erase(p.begin() + i);
					}
					else {
						p.erase(p.begin() + i - 1);
						p.erase(p.begin() + i - 1);
					}
				}
				p.push_back(make_pair(make_pair(start, end), cost));
				p.push_back(make_pair(make_pair(end, start), cost));

				break;
			}
		}
		if (!flag) {
			p.push_back(make_pair(make_pair(start, end), cost));
			p.push_back(make_pair(make_pair(end, start), cost));
		}
	//	print_table();
	}
	void update_graph(int sender, int receiver) {
	//	print_path_vector();
	//	printf("\n\n\n\n\n");
		for (int i = 0; i < 100; i++)
			routing_table[i].update_vector(p, i, sender, receiver);
		//print_path_vector();
		exchange_table();
	}
};
 
void print_message(graph &G) {
	message_file = fopen("messages.txt", "r");
	int sender, receiver;
	char input[101][101];
	int j = 0;
	while (fscanf(message_file, "%d %d %[^'\n']s", &sender, &receiver, input[j]) != EOF) {
		//printf("%d %d %s\n", sender, receiver, input[j]);
		int tracking[100];
		for (int i = 0; i < 100; i++)
			tracking[i] = MAX_VAL;
		int cost = G.track_path(tracking, sender, receiver);
		if (cost > 0)
			printf("from %d to %d cost %d hops ", sender, receiver, cost);
		else {
			printf("from %d to %d cost infinite hops unreachable ", sender, receiver);
		}
		int k = 0;
		while (tracking[k] != MAX_VAL) {
			printf("%d ", tracking[k++]);
		}
		printf("message %s\n", input[j]);
		j++;
	}
	fclose(message_file);
}

int main(int argc, char*argv[]) {

	/*
	if (argc != 3) {
		printf("usage: distvec topologyfile messagefile changesfile\n");
		return 0;
	}
	input_file = fopen(argv[1], "r");
	message_file = fopen(argv[2], "r");
	change_file = fopen(argv[3], "r");

	if (input_file == NULL || message_file == NULL || change_file == NULL) {
		printf("Error: open input file.\n");
		return 0;
	}
	printf("Complete.Output file written to output_dv.txt.\n");
	*/
	input_file = fopen("topology.txt", "r");
	change_file = fopen("changes.txt", "r");
	graph G;
	fscanf(input_file, "%d", &G.node_number);
	//printf("G.node: %d\n", G.node_number);
	int first, sec, value;
	while (fscanf(input_file, "%d %d %d", &first, &sec, &value) != EOF) {
		//printf("%d %d %d\n", first, sec, value);
		G.initialize(first, sec, value);
	}
	G.make_all_table();
	G.exchange_table();
	G.print_table();

	print_message(G);
	G.make_path_vector();
	int send, receive, val;
	while (fscanf(change_file, "%d %d %d", &send, &receive, &val) != EOF) {
		//printf("%d %d %d\n", send, receive, val);
		if (val != -999) {
			G.change_table(send, receive, val);
			G.exchange_table();
		}
		else { // route 제거 path_vector 필요함
			G.change_table(send, receive, val); // 정보 바꾸고
			G.update_graph(send, receive); // update vector
			//G.make_path_vector();
			//G.print_path_vector();
		}
		G.make_path_vector();
	//	G.print_path_vector();
		G.print_table();
		print_message(G);
	}
	//output_file = fopen("output_dv.txt", "w");
	//printf("successfully terminated\n");
	fclose(input_file);
	fclose(change_file);
//	fclose(output_file);

	return 0;
}