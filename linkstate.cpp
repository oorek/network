#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <utility> 
#include <vector>
#include <string>
#include <queue>
using namespace std;
typedef pair<int, int> ii;
FILE* input_file, * message_file, * change_file;
FILE* output_file;
#define MAX_VAL 101
#define MAX_COST 100000000

class graph {
public:
	int node_number;
	vector<pair<ii, int>> p;
	int** distance;
	int** parent;
	graph() {
		distance = (int**)malloc(sizeof(int*) * 100);
		for (int i = 0; i < 100; i++)
			distance[i] = (int*)malloc(sizeof(int) * 101);
		for (int i = 0; i < 100; i++) {
			for (int j = 0; j < 101; j++) {
				distance[i][j] = MAX_COST;
			}
		}
		parent = (int**)malloc(sizeof(int*) * 100);
		for (int i = 0; i < 100; i++)
			parent[i] = (int*)malloc(sizeof(int) * 101);
		for (int i = 0; i < 100; i++) {
			for (int j = 0; j < 101; j++) {
				parent[i][j] = MAX_VAL;
			}
		}
	}
	~graph() {
		for (int i = 0; i < 100; i++)
			free(distance[i]);
		free(distance);
		for (int i = 0; i < 100; i++)
			free(parent[i]);
		free(parent);
	}
	void initialize(int first, int sec, int value) {
		p.push_back(make_pair(make_pair(first, sec), value));
		p.push_back(make_pair(make_pair(sec, first), value));
	}
	void dijkstra() {
		for (int i = 0; i < 100; i++) {
			for (int j = 0; j < 101; j++) {
				distance[i][j] = MAX_COST;
			}
		}
		for (int i = 0; i < 100; i++) {
			for (int j = 0; j < 101; j++) {
				parent[i][j] = MAX_VAL;
			}
		}

		for (int i = 0; i < node_number; i++) {
			int K = i; // 시작 노드
			// <거리,  node>
			priority_queue<ii, vector<ii>, greater<ii>>pq;
			bool visited[100] = {0,};
			distance[K][K] = 0;
			parent[K][K] = K;
			pq.push(ii(distance[K][K], K));
			while (!pq.empty()) {
				ii A = pq.top();
				int cur = A.second;
				pq.pop();
				if (!visited[cur]) {
					visited[cur] = true;
					for (int j = 0; j < p.size(); j++) {
						if (p[j].first.first == cur) {
							int next = p[j].first.second;
							int cost = p[j].second;
							if (distance[K][next] > distance[K][cur] + cost) {
								distance[K][next] = distance[K][cur] + cost;
								parent[K][next] = cur;
								pq.push(ii(distance[K][next], next));
							}
							if (distance[K][next] == distance[K][cur] + cost) {
								if(parent[K][next] > cur)
									parent[K][next] = cur;
							}
						}
					}
				}
			}
		}
	}
	int find_parent(int i, int j) {
		int par;
		while (1) {
			par = parent[i][j];
			if (par == i)
				break;
			j = par;
		}
		return j;
	}
	void print_table() {
		for (int i = 0; i < node_number; i++) {
			for(int j=0; j<node_number; j++)
				printf("%d %d %d\n", j, find_parent(i,j), distance[i][j]);
			printf("\n");
		}
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
			return;
		}
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
	}
	int track_path(int *tracking, int sender, int receiver) {
		int par;
		int m = 0;
		int i = sender;
		int j = receiver;
		while (1) {
			par = parent[i][j];
			if (par == i) {
				tracking[m++] = i;
				break;
			}
			j = par;
			tracking[m++] = j;
		}
		return  distance[sender][receiver];
	}
};

void print_message(graph& G) {
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
		int k = 99;
		while (tracking[k] == MAX_VAL) {
			k--;
		}
		for(int i=k; i>=0; i--)
			printf("%d ", tracking[i]);
		printf("message %s\n", input[j]);
		j++;
	}
	fclose(message_file);
}

int main(int argc, char* argv[]) {

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
	G.dijkstra();
	G.print_table();
	print_message(G);
	int send, receive, val;
	while (fscanf(change_file, "%d %d %d", &send, &receive, &val) != EOF) {
		//printf("%d %d %d\n", send, receive, val);
		if (val != -999) {
			G.change_table(send, receive, val);
			G.dijkstra();
			G.print_table();
		}
		else {
			G.change_table(send, receive, val);
			G.dijkstra();
			G.print_table();
		}
		print_message(G);
	}


	fclose(input_file);
	fclose(change_file);
	//	fclose(output_file);

	return 0;
}