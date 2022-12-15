#!/usr/bin/env bash


benchmarks_path="./benchmarks/"


#read number_of_benchmarks <$benchmarks_path"number.txt"


results_path="./results/"
method_name="proposed"
for((i = 0; i < 240; ++i)); do
  cascade_path=$benchmarks_path"benchmark"$i".txt"
  log_file=$results_path"log"$i".txt"
  echo $log_file
  echo "\"method_name\":\"$method_name\",">$log_file
  args="-c $cascade_path -l $log_file"

  timeout 60s ./cmake-build-release/run_benchmark $args
  exit_code=$?

  if (( exit_code==124 )); then
    echo "\"timeout\",">>$log_file
  elif (( exit_code==134 )); then
    echo "\"memout\",">>$log_file
  fi
done

results_path="./results_KN/"
method_name="KN"
for((i = 0; i < 240; ++i)); do
  cascade_path=$benchmarks_path"benchmark"$i".txt"
  log_file=$results_path"log"$i".txt"
  echo $log_file
  echo "\"method_name\":\"$method_name\",">$log_file
  args="-c $cascade_path -l $log_file"

  timeout 60s ./cmake-build-release/run_benchmark_KN $args
  exit_code=$?

  if (( exit_code==124 )); then
    echo "\"timeout\",">>$log_file
  elif (( exit_code==134 )); then
    echo "\"memout\",">>$log_file
  fi
done