#include <iostream>
#include <vector>
#include <omp.h>
#include <fstream>
#include <algorithm>
#include <string>

using namespace std;

// 1. Função para carregar o arquivo e dividi-lo em K sublistas ordenadas (AGORA USANDO STRING)
vector<vector<string>> load_runs_from_file(const string &filepath, int k_ways)
{
  ifstream file(filepath);
  vector<string> all_data;
  string line;

  if (!file.is_open())
  {
    cerr << "[Erro] Nao foi possivel abrir o arquivo: " << filepath << endl;
    cerr << "-> Para testar sem o arquivo, o programa gerara caminhos ficticios." << endl;
    srand(42);
    for (int i = 0; i < 10000; i++)
      all_data.push_back("/ficticio/path/log_" + to_string(rand() % 100000) + ".txt");
  }
  else
  {
    while (getline(file, line))
    {
      if (!line.empty())
      { 
        all_data.push_back(line);
      }
    }
    file.close();
  }

  vector<vector<string>> runs(k_ways);
  if (all_data.empty())
    return runs;

  int chunk_size = all_data.size() / k_ways;
  if (chunk_size == 0)
    chunk_size = 1; 

  for (int i = 0; i < k_ways; i++)
  {
    int start_idx = i * chunk_size;
    int end_idx = (i == k_ways - 1) ? all_data.size() : (i + 1) * chunk_size;

    if (start_idx >= all_data.size())
      break;

    for (int j = start_idx; j < end_idx; j++)
    {
      runs[i].push_back(all_data[j]);
    }
    // O C++ entende automaticamente que deve ordenar strings em ordem alfabética
    sort(runs[i].begin(), runs[i].end());
  }

  return runs;
}

// 2. Função sequencial básica de fusão de duas listas de STRINGS
vector<string> merge_two_lists(const vector<string> &left, const vector<string> &right)
{
  vector<string> result;
  result.reserve(left.size() + right.size());
  size_t i = 0, j = 0;

  // A comparação <= também funciona para strings (ordem léxica)
  while (i < left.size() && j < right.size())
  {
    if (left[i] <= right[j])
      result.push_back(left[i++]);
    else
      result.push_back(right[j++]);
  }
  while (i < left.size())
    result.push_back(left[i++]);
  while (j < right.size())
    result.push_back(right[j++]);

  return result;
}

// 3. Algoritmo de Merge Tree Paralelo (Divisão e Conquista com STRINGS)
vector<string> parallel_k_way_merge(vector<vector<string>> &lists, int start, int end)
{
  if (start == end)
    return lists[start];
  if (start + 1 == end)
    return merge_two_lists(lists[start], lists[end]);

  int mid = start + (end - start) / 2;
  vector<string> left_result, right_result;

  if ((end - start) < 4)
  {
    left_result = parallel_k_way_merge(lists, start, mid);
    right_result = parallel_k_way_merge(lists, mid + 1, end);
  }
  else
  {
#pragma omp task shared(left_result)
    left_result = parallel_k_way_merge(lists, start, mid);

#pragma omp task shared(right_result)
    right_result = parallel_k_way_merge(lists, mid + 1, end);

#pragma omp taskwait
  }

  return merge_two_lists(left_result, right_result);
}

int main()
{
  string arquivo_tp1 = "./listagem.txt";
  int num_vias = 16;

  cout << "Lendo arquivo de logs/diretorios (" << arquivo_tp1 << ")..." << endl;

  vector<vector<string>> runs = load_runs_from_file(arquivo_tp1, num_vias);

  // Filtra vias vazias (caso o arquivo tenha menos de 16 linhas)
  vector<vector<string>> valid_runs;
  for (const auto &r : runs)
  {
    if (!r.empty())
      valid_runs.push_back(r);
  }

  if (valid_runs.empty())
  {
    cout << "[Erro] Nao ha dados para ordenar!" << endl;
    return 1;
  }

  vector<string> final_sorted_list;
  double start_time = omp_get_wtime();

#pragma omp parallel
  {
#pragma omp single
    {
      final_sorted_list = parallel_k_way_merge(valid_runs, 0, valid_runs.size() - 1);
    }
  }

  double end_time = omp_get_wtime();
  cout << "Tempo paralelo OpenMP: " << (end_time - start_time) << " segundos." << endl;

  // Imprime os caminhos em formato de lista 
  cout << "\nLista final ordenada (Primeiros 10 diretorios): [" << endl;
  for (int i = 0; i < min(10, (int)final_sorted_list.size()); i++)
  {
    cout << "  " << final_sorted_list[i] << endl;
  }
  cout << "  ..." << endl
       << "]" << endl;
  cout << "Total de diretorios processados: " << final_sorted_list.size() << endl;

  return 0;
}