#include <iostream>
#include <cstdlib>
#include <vector>
#include <omp.h>
#include <cmath>
#include <ctime>

using namespace std;

struct Point
{
  float x, y;
};

struct QuadNode
{
  float x_min, x_max, y_min, y_max;
  vector<Point> points;
  QuadNode *nw, *ne, *sw, *se;
  bool is_leaf;

  QuadNode(float x1, float x2, float y1, float y2)
      : x_min(x1), x_max(x2), y_min(y1), y_max(y2),
        nw(nullptr), ne(nullptr), sw(nullptr), se(nullptr), is_leaf(true) {}
};

const int CAPACITY = 50; // Cutoff

void build_quadtree_parallel(QuadNode *node, vector<Point> &pts)
{
  if (pts.size() <= CAPACITY)
  {
    node->points = pts;
    node->is_leaf = true;
    return;
  }

  node->is_leaf = false;
  float x_mid = (node->x_min + node->x_max) / 2.0f;
  float y_mid = (node->y_min + node->y_max) / 2.0f;

  // Pré-divisão sequencial local (Pre-splitting)
  vector<Point> p_nw, p_ne, p_sw, p_se;
  for (const auto &p : pts)
  {
    if (p.x <= x_mid && p.y >= y_mid)
      p_nw.push_back(p);
    else if (p.x > x_mid && p.y >= y_mid)
      p_ne.push_back(p);
    else if (p.x <= x_mid && p.y < y_mid)
      p_sw.push_back(p);
    else
      p_se.push_back(p);
  }

  node->nw = new QuadNode(node->x_min, x_mid, y_mid, node->y_max);
  node->ne = new QuadNode(x_mid, node->x_max, y_mid, node->y_max);
  node->sw = new QuadNode(node->x_min, x_mid, node->y_min, y_mid);
  node->se = new QuadNode(x_mid, node->x_max, node->y_min, y_mid);

// Divisão livre de Locks
#pragma omp task shared(p_nw)
  build_quadtree_parallel(node->nw, p_nw);

#pragma omp task shared(p_ne)
  build_quadtree_parallel(node->ne, p_ne);

#pragma omp task shared(p_sw)
  build_quadtree_parallel(node->sw, p_sw);

#pragma omp task shared(p_se)
  build_quadtree_parallel(node->se, p_se);

#pragma omp taskwait
}

// Verifica interseção do Bounding Box (Pruning)
bool intersects(QuadNode *node, Point center, float radius)
{
  float closestX = max(node->x_min, min(center.x, node->x_max));
  float closestY = max(node->y_min, min(center.y, node->y_max));
  float dx = center.x - closestX;
  float dy = center.y - closestY;
  return (dx * dx + dy * dy) <= (radius * radius);
}

void query_tree(QuadNode *node, Point center, float radius, vector<Point> &local_result)
{
  if (!node || !intersects(node, center, radius))
    return;

  if (node->is_leaf)
  {
    for (const auto &p : node->points)
    {
      float dx = center.x - p.x;
      float dy = center.y - p.y;
      if (dx * dx + dy * dy <= radius * radius)
      {
        local_result.push_back(p);
      }
    }
    return;
  }

  query_tree(node->nw, center, radius, local_result);
  query_tree(node->ne, center, radius, local_result);
  query_tree(node->sw, center, radius, local_result);
  query_tree(node->se, center, radius, local_result);
}

int main()
{
  srand(time(NULL));

  vector<Point> particles(100000);

  for (int i = 0; i < 100000; ++i)
  {
    particles[i].x = static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / 1000.0f));
    particles[i].y = static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / 1000.0f));
  }

  QuadNode *root = new QuadNode(0, 1000, 0, 1000);

#pragma omp parallel
  {
#pragma omp single
    {
#pragma omp taskgroup
      build_quadtree_parallel(root, particles);
    }
  }

  vector<Point> query_centers = {{500, 500}, {100, 100}, {900, 900}};
  float influence_radius = 50.0f;

// Escalonamento Dinâmico para balancear a densidade das regiões
#pragma omp parallel for schedule(dynamic)
  for (size_t i = 0; i < query_centers.size(); ++i)
  {
    vector<Point> thread_local_results;
    query_tree(root, query_centers[i], influence_radius, thread_local_results);

#pragma omp critical
    {
      cout << "[Consulta] Ponto central (" << query_centers[i].x << ", " << query_centers[i].y
           << ") -> Partículas encontradas no raio de " << influence_radius
           << " = " << thread_local_results.size() << endl;
    }
  }

  return 0;
}