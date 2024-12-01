echo -e "\n==================== [Paralelo] NOVA EXECUCAO ==================== \n" >> "resultados.txt"

for j in {1..5}; do
    OMP_NUM_THREADS=$((j*4 ))
    echo  ">>NUM_THREADS= $OMP_NUM_THREADS" >> "resultados.txt"
    for i in {1..5}; do
        n=$((90000 * i ))  
        m=$((i * 200  ))      
        r=$(((i % 5) + 1))
        
        ./dijkstra "$n" "$m" "$r" >> "resultados.txt"
    done
done
