#!/bin/bash

IMAGE_NAME="tp3-prof-ciencia-computacao"

case "$1" in
  build)
    echo "Construindo imagem Docker..."
    docker build -t $IMAGE_NAME .
    ;;
  run)
    echo "Iniciando container interativo..."
    docker run -it -v $(pwd):/project $IMAGE_NAME
    ;;
  all)
    echo "Rodando todos os exercícios automaticamente..."
    docker run -v $(pwd):/project $IMAGE_NAME bash -c "make exec_all"
    ;;
  clean)
    echo "Forçando limpeza dos binários..."
    docker run -v $(pwd):/project $IMAGE_NAME bash -c "make clean"
    ;;
  *)
    echo "Uso: ./run.sh {build|run|all|clean}"
    echo "  build: Cria a imagem"
    echo "  run:   Entra no container para usar o VI"
    echo "  all:   Compila, roda tudo e apaga os executáveis no final"
    echo "  clean: Apaga manualmente os executáveis gerados (se necessário)"
    ;;
esac