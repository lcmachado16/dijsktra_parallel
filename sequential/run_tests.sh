echo -e "\n==================== [Sequencial] NOVA EXECUCAO ==================== \n" >> "resultados.txt"

for i in {1..5}; do
    n=$((90000 * i ))  
    m=$((i * 200  ))      
    r=$(((i % 5) + 1))
    
    ./dijkstra_sequential "$n" "$m" "$r" >> "resultados.txt"
done