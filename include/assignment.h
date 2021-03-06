/**
 * MIT License
 *
 * Copyright (c) 2020  H.Y. Pei
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 * 
 */

#pragma once

#include "HungarianAlg.h"
#include <vector>
#include <string>
#include <functional>

namespace ele {
static bool AddNeighbour( int egoIndex,
    std::vector<bool>& egoSelected, std::vector<std::vector<int> >& egoIndices,
    std::vector<int>& egoBlocks, std::vector<bool>& neighbourSelected,
    std::vector<std::vector<int> >& neighbourIndices, 
		std::vector<int>& neighbourBlocks) {
  // egoIndex has been used
  if (egoSelected[egoIndex]) {
    return false;
  } 
  egoSelected[egoIndex] = true;  
  // no matched element
  if (egoIndices[egoIndex].empty()) {
    return false;
  }
  egoBlocks.emplace_back(egoIndex);  
  for (int neighbour_idx=0; neighbour_idx<egoIndices[egoIndex].size(); 
      neighbour_idx++) {
    AddNeighbour(egoIndices[egoIndex][neighbour_idx], neighbourSelected, 
				neighbourIndices, neighbourBlocks, egoSelected, egoIndices, egoBlocks);
  }
  return true;
}

template<typename LeftClass, typename RightClass>
class Assignment {
public:
	// Assignment() {}
	Assignment( std::vector<LeftClass>* left, std::vector<RightClass>* right,
			std::function<double(LeftClass&, RightClass&)> func, double dist_thresh): 
					M_(left->size()),	N_(right->size()),	dist_thresh_(dist_thresh),                
					left_list_(left), right_list_(right), calculate_dist_(func),
					left_selected_(M_, false), right_selected_(N_, false), 
					left_indices_(M_), right_indices_(N_), dist_matrix_(M_*N_),
					assignment_result_(M_, -1) {}

  ~Assignment() {}

public:
  void Solve() {
    CalculateDistance();
    int block_num = GetBlocks();
    if (block_num < 1) { return ; }
    for (size_t i=0; i<block_num; i++) {
      if (left_blocks_[i].empty() || right_blocks_[i].empty() ) {
        continue;
      }

      // case : one vs one
      if (1 == left_blocks_[i].size() && 1 == right_blocks_[i].size()) {
        assignment_result_[left_blocks_[i].at(0)] = right_blocks_[i].at(0);
        continue;
      }

      // case : one vs multi
      if ( 1 == left_blocks_[i].size() && 1 < right_blocks_[i].size() ) {
        size_t offset = left_blocks_[i].at(0) * N_; 
        double nearest_dist = dist_matrix_[offset + right_blocks_[i].at(0)];
        int nearest_index = right_blocks_[i].at(0);
        for (size_t j=1; j<right_blocks_[i].size(); j++) {
          if ( nearest_dist < dist_matrix_[offset + right_blocks_[i].at(j)] ) {
            right_selected_[right_blocks_[i].at(j)] = false;
          } else {
            right_selected_[nearest_index] = false;
            nearest_index = right_blocks_[i].at(j);
            nearest_dist = dist_matrix_[offset + right_blocks_[i].at(j)];
          }
        }
        assignment_result_[left_blocks_[i].at(0)] = nearest_index;
        continue;
      }
      if ( 1 < left_blocks_[i].size() && 1 == right_blocks_[i].size() ) {
        double nearest_dist = dist_matrix_[left_blocks_[i].at(0) * N_  
            + right_blocks_[i].at(0)];
        int nearest_index = left_blocks_[i].at(0);
        for (size_t j=1; j<left_blocks_[i].size(); j++) {
          if ( nearest_dist < dist_matrix_[left_blocks_[i].at(j) * N_  
              + right_blocks_[i].at(0)] ) {
            left_selected_[left_blocks_[i].at(j)] = false;
          } else {
            left_selected_[nearest_index] = false;
            nearest_index = left_blocks_[i].at(j);
            nearest_dist = dist_matrix_[left_blocks_[i].at(j) * N_  
                + right_blocks_[i].at(0)];
          }
        }
        assignment_result_[nearest_index] = right_blocks_[i].at(0);
        continue;
      }

      // case : multi vs multi
      // if ( 1 < left_blocks_[i].size() && 1 < right_blocks_[i].size() )     
      size_t left_size = left_blocks_[i].size();
      size_t right_size = right_blocks_[i].size();
      std::vector<double> sub_dist_matrix(left_size*right_size);
      for (size_t k=0; k<left_size; k++) {
        for (size_t j=0; j<right_size; j++) {
          sub_dist_matrix[k*right_size+j] = dist_matrix_[left_blocks_[i].at(k) 
							* N_ + right_blocks_[i].at(j)];
        }
      }
      AssignmentProblemSolver km;
      std::vector<int> sub_assignment(left_size);
      km.Solve(sub_dist_matrix, left_size, right_size, sub_assignment, 
          AssignmentProblemSolver::optimal);
      for (size_t j=0; j<right_size; j++ ) {
        right_selected_[right_blocks_[i].at(j)] = false;
      }
      for (size_t j=0; j<left_size; j++ ) {
        if (-1 == sub_assignment[j]) {
          left_selected_[left_blocks_[i].at(j)] = false;
        } else {
          right_selected_[right_blocks_[i].at(sub_assignment[j])] = true;
          assignment_result_[left_blocks_[i].at(j)] = 
							right_blocks_[i].at(sub_assignment[j]);
        }
      }
    }
  }

  std::vector<int>& Assign() { return assignment_result_; }
  std::vector<bool>& LeftMatched() { return left_selected_; }
  std::vector<bool>& RightMatched() { return right_selected_; }
  std::vector<bool>& LeftSelected() { return left_selected_; }
  std::vector<bool>& RightSelected() { return right_selected_; }
  std::vector<std::vector<int> >& LeftIndices() { return left_indices_; }
  std::vector<std::vector<int> >& RightIndices() { return right_indices_; }
  std::vector<std::vector<int> >& LeftBlocks() { return left_blocks_; }
  std::vector<std::vector<int> >& RightBlocks() { return right_blocks_; }

private:    
  void CalculateDistance() {
    for (size_t i=0; i<M_; i++) {
      size_t offset = i * N_; 
      for (size_t j=0; j<N_; j++) {
        dist_matrix_[offset+j] = calculate_dist_(left_list_->at(i), 
            right_list_->at(j));
        if (dist_matrix_[offset+j] < dist_thresh_ ) {
          left_indices_[i].emplace_back(j);
          right_indices_[j].emplace_back(i);
        }
      }
    }
  }

  int  GetBlocks() {
    int block_index = 0;
    int max_size = std::max(M_, N_); 
    left_blocks_.resize(max_size);
    right_blocks_.resize(max_size);
    for (int i=0; i<left_indices_.size(); i++) {
      AddNeighbour(i, left_selected_, left_indices_, left_blocks_[block_index], 
          right_selected_, right_indices_, right_blocks_[block_index] );
      if (!left_blocks_[block_index].empty() 
				  || !right_blocks_[block_index].empty()) {
        block_index++;
      }
    }
    return block_index;
  }
    
private:
  size_t M_;
  size_t N_;
  double dist_thresh_;
  std::vector<LeftClass>* left_list_;
  std::vector<RightClass>* right_list_;
  std::function<double(LeftClass&, RightClass&)> calculate_dist_;
  std::vector<double> dist_matrix_;
  std::vector<bool> left_selected_;
  std::vector<bool> right_selected_;
  std::vector<std::vector<int> > left_indices_;
  std::vector<std::vector<int> > right_indices_;
  std::vector<std::vector<int> > left_blocks_;
  std::vector<std::vector<int> > right_blocks_;
  std::vector<int> assignment_result_;

}; // class Assignment
} // !namespace ele
