#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <map>


struct super {
	unsigned long magic_num;
	unsigned long inodes_count;
	unsigned long blocks_count;
	unsigned long block_size;
	unsigned long frag_size;
	unsigned long blocks_per_group;
	unsigned long inodes_per_group;
	unsigned long frags_per_group;
	unsigned long first_data_block;

	void readrow(std::istream& in) {

		std::string line;
		std::getline(in, line, '\n');
		//if (line == "")return;
		std::stringstream row(line);
		std::string element;

		std::getline(row, element, ',');
		magic_num = std::stoul(element, nullptr, 16);

		std::getline(row, element, ',');
		inodes_count = std::stoul(element, nullptr, 10);

		std::getline(row, element, ',');
		blocks_count = std::stoul(element, nullptr, 10);

		std::getline(row, element, ',');
		block_size = std::stoul(element, nullptr, 10);

		std::getline(row, element, ',');
		frag_size = std::stoul(element, nullptr, 10);

		std::getline(row, element, ',');
		blocks_per_group = std::stoul(element, nullptr, 10);

		std::getline(row, element, ',');
		inodes_per_group = std::stoul(element, nullptr, 10);

		std::getline(row, element, ',');
		frags_per_group = std::stoul(element, nullptr, 10);

		std::getline(row, element, ',');
		first_data_block = std::stoul(element, nullptr, 10);
	}

};

std::istream& operator>>(std::istream& in, super& data) {
	data.readrow(in);
	return in;
}

struct group_des {
	unsigned long contained_blocks;
	unsigned long free_block_num;
	unsigned long free_inodes_num;
	unsigned long dir_num;
	unsigned long inode_bit_block;
	unsigned long block_bit_block;
	unsigned long inode_table_start_block;

	void readrow(std::istream& in) {
		std::string line;
		std::getline(in, line, '\n');

		std::stringstream row(line);
		std::string element;

		std::getline(row, element, ',');
		contained_blocks = std::stoul(element, nullptr, 10);

		std::getline(row, element, ',');
		free_block_num = std::stoul(element, nullptr, 10);

		std::getline(row, element, ',');
		free_inodes_num = std::stoul(element, nullptr, 10);

		std::getline(row, element, ',');
		dir_num = std::stoul(element, nullptr, 10);

		std::getline(row, element, ',');
		inode_bit_block = std::stoul(element, nullptr, 16);

		std::getline(row, element, ',');
		block_bit_block = std::stoul(element, nullptr, 16);

		std::getline(row, element, ',');
		inode_table_start_block = std::stoul(element, nullptr, 16);
	}
};

std::istream& operator>>(std::istream& in, group_des& data) {
	data.readrow(in);
	return in;
}

struct bitmap {
	std::map<unsigned long, std::vector<unsigned long>> bit;

	void readrow(std::istream& in) {
		std::string line;
		std::getline(in, line, '\n');

		std::stringstream row(line);
		std::string block_of_map;
		std::string entry_num;

		std::getline(row, block_of_map, ',');
		std::getline(row, entry_num, ',');

		bit[std::stoul(block_of_map, nullptr, 16)].push_back(std::stoul(entry_num, nullptr, 10));

	}

};

std::istream& operator>>(std::istream& in, bitmap& data) {
	data.readrow(in);
	return in;
}

struct inode {
	unsigned long inode_num;
	char file_type;
	unsigned long mode;
	unsigned long uid;
	unsigned long gid;
	unsigned long link_count;
	unsigned long ctime;
	unsigned long mtime;
	unsigned long atime;
	unsigned long file_size;
	unsigned long num_of_blocks;
	std::vector<unsigned long> block_pt = std::vector<unsigned long>(15);

	void readrow(std::istream& in) {
		std::string line;
		std::getline(in, line, '\n');

		std::stringstream row(line);
		std::string element;

		std::getline(row, element, ',');
		inode_num = std::stoul(element, nullptr, 10);

		std::getline(row, element, ',');
		file_type = element[0];

		std::getline(row, element, ',');
		mode = std::stoul(element, nullptr, 8);

		std::getline(row, element, ',');
		uid = std::stoul(element, nullptr, 10);

		std::getline(row, element, ',');
		gid = std::stoul(element, nullptr, 10);

		std::getline(row, element, ',');
		link_count = std::stoul(element, nullptr, 10);

		std::getline(row, element, ',');
		ctime = std::stoul(element, nullptr, 16);

		std::getline(row, element, ',');
		mtime = std::stoul(element, nullptr, 16);

		std::getline(row, element, ',');
		atime = std::stoul(element, nullptr, 16);

		std::getline(row, element, ',');
		file_size = std::stoul(element, nullptr, 10);

		std::getline(row, element, ',');
		num_of_blocks = std::stoul(element, nullptr, 10);

		for (size_t i = 0; i < block_pt.size(); i++) {
			std::getline(row, element, ',');
			block_pt[i] = std::stoul(element, nullptr, 16);
		}
	}

};

std::istream& operator>>(std::istream& in, inode& data) {
	data.readrow(in);
	return in;
}

struct dir {
	unsigned long parent_inode_num;
	unsigned long entry_num;
	unsigned long entry_len;
	unsigned long name_len;
	unsigned long inode_num;
	std::string name;

	void readrow(std::istream& in) {
		std::string line;
		std::getline(in, line, '"');
		std::getline(in, name, '"');
		in.get();//get rid of newline

		std::stringstream row(line);
		std::string element;

		std::getline(row, element, ',');
		parent_inode_num = std::stoul(element, nullptr, 10);

		std::getline(row, element, ',');
		entry_num = std::stoul(element, nullptr, 10);

		std::getline(row, element, ',');
		entry_len = std::stoul(element, nullptr, 10);

		std::getline(row, element, ',');
		name_len = std::stoul(element, nullptr, 10);

		std::getline(row, element, ',');
		inode_num = std::stoul(element, nullptr, 10);


	}

};

std::istream& operator>>(std::istream& in, dir& data) {
	data.readrow(in);
	return in;
}

struct indirect {


	std::map<unsigned long, std::map<unsigned long, unsigned long>> indir;
	void readrow(std::istream& in) {
		unsigned long block_num_of_contained_block;
		unsigned long entry_num;
		unsigned long block_pt_val;

		std::string line;
		std::getline(in, line, '\n');

		std::stringstream row(line);
		std::string element;

		std::getline(row, element, ',');
		block_num_of_contained_block = std::stoul(element, nullptr, 16);

		std::getline(row, element, ',');
		entry_num = std::stoul(element, nullptr, 10);

		std::getline(row, element, ',');
		block_pt_val = std::stoul(element, nullptr, 16);

		indir[block_num_of_contained_block][block_pt_val] = entry_num;
	}

};

std::istream& operator>>(std::istream& in, indirect& data) {
	data.readrow(in);
	return in;
}



const std::map<unsigned long, unsigned long>& find_block_list_indirect(unsigned long block_num, const indirect& indirectdata) {

	return indirectdata.indir.at(block_num);
}

//This function get a map for an entry in directory.csv with key=block_num; val=a vector of entry_num; if there is dup then the size of the vector is greater than 1
std::map<unsigned long, std::vector<unsigned long>> get_block_list(const inode& entry, const indirect& indirectdata) {
	std::map<unsigned long, std::vector<unsigned long>> block_list;
	unsigned long entry_num = 0;
	for (size_t i = 0; i < 15; i++) {
		if (entry.block_pt[i] != 0) {
			block_list[entry.block_pt[i]].push_back(entry_num);
		}
		entry_num++;
	}
	//continue to get first level indirect block
	if (entry.block_pt[12] != 0) {
		unsigned long pt = entry.block_pt[12];
		const std::map<unsigned long, unsigned long> entry_under_block = find_block_list_indirect(pt, indirectdata);
		for (auto it = entry_under_block.begin(), it_end = entry_under_block.end(); it != it_end; ++it) {
			if (it->first != 0) {
				block_list[it->first].push_back(it->second);
			}
		}
	}
	//double
	if (entry.block_pt[13] != 0) {
		unsigned long pt = entry.block_pt[13];
		const std::map<unsigned long, unsigned long> entry_under_block = find_block_list_indirect(pt, indirectdata);
		for (auto it = entry_under_block.begin(), it_end = entry_under_block.end(); it != it_end; ++it) {
			if (it->first != 0) {
				block_list[it->first].push_back(it->second);
				unsigned long next_pt = it->first;
				const std::map<unsigned long, unsigned long> entry_under_block_2l = find_block_list_indirect(next_pt, indirectdata);
				for (auto ater = entry_under_block_2l.begin(), ater_end = entry_under_block_2l.end(); ater != ater_end; ++ater) {
					if (ater->first != 0) {
						block_list[ater->first].push_back(ater->second);
					}
				}
			}
		}
	}
	//triple
	if (entry.block_pt[14] != 0) {
		unsigned long pt = entry.block_pt[14];
		const std::map<unsigned long, unsigned long> entry_under_block = find_block_list_indirect(pt, indirectdata);
		for (auto it = entry_under_block.begin(), it_end = entry_under_block.end(); it != it_end; ++it) {
			if (it->first != 0) {
				block_list[it->first].push_back(it->second);
				unsigned long next_pt = it->first;
				const std::map<unsigned long, unsigned long> entry_under_block_2l = find_block_list_indirect(next_pt, indirectdata);
				for (auto ater = entry_under_block_2l.begin(), ater_end = entry_under_block_2l.end(); ater != ater_end; ++ater) {
					if (ater->first != 0) {
						block_list[ater->first].push_back(ater->second);
						unsigned long tri_pt = ater->first;
						const std::map<unsigned long, unsigned long> entry_under_block_3l = find_block_list_indirect(tri_pt, indirectdata);
						for (auto itater = entry_under_block_3l.begin(), itater_end = entry_under_block_3l.end(); itater != itater_end; ++itater) {
							if (itater->first != 0) {
								block_list[itater->first].push_back(itater->second);
							}
						}
					}
				}
			}
		}
	}
	return block_list;
}

int main() {
	std::ofstream ofile;
	std::fstream file;
	file.open("super.csv");
	std::vector<super> superdata;

	while (file.peek() != EOF) {
		super row_data;
		file >> row_data;
		superdata.push_back(row_data);
	}
	file.close();

	file.open("group.csv");
	std::vector<group_des> groupdata;
	while (file.peek() != EOF) {
		group_des row_data;
		file >> row_data;
		groupdata.push_back(row_data);
	}
	file.close();

	file.open("bitmap.csv");
	//std::vector<bitmap> bitdata;
	bitmap bitdata;
	while (file.peek() != EOF) {
		//bitmap row_data;
		file >> bitdata;
		//bitdata.push_back(row_data);
	}
	file.close();

	file.open("inode.csv");
	std::vector<inode> indata;
	while (file.peek() != EOF) {
		inode row_data;
		file >> row_data;
		indata.push_back(row_data);
	}
	file.close();

	file.open("directory.csv");
	std::vector<dir> dirdata;

	while (file.peek() != EOF) {

		dir row_data;
		file >> row_data;
		dirdata.push_back(row_data);
	}
	file.close();

	file.open("indirect.csv");

	indirect indirectdata;
	while (file.peek() != EOF) {
		
		file >> indirectdata;
	}
	file.close();

	ofile.open("lab3b_check.txt");

	//map the child block ptr to a vecotr of pair with parent ptr and entry, this is all the block ptrs in indirect.csv
	std::map<unsigned long, std::vector<std::pair<unsigned long, unsigned long>>> indir_child_parent_entry;
	for (auto it = indirectdata.indir.begin(), it_end = indirectdata.indir.end();it != it_end; ++it) {
		unsigned long parent_ptr = it->first;
		const auto& map = it->second;
		for (auto ater = map.begin(), ater_end = map.end();ater != ater_end; ++ater) {
			unsigned long entry_num = ater->second;
			unsigned long child_ptr = ater->first;
			indir_child_parent_entry[child_ptr].push_back(std::pair<unsigned long, unsigned long>(parent_ptr, entry_num));
		}
	}
	//a map with key is the block number, and get a vector of pairs(inode & entry) this is the (all the block ptrs) data in inode.csv
	std::map<unsigned long, std::vector<std::pair<unsigned long, unsigned long>>> directory_blockptr_inode_entry;
	for (size_t i = 0;i < indata.size();i++) {
		for (size_t j = 0;j < 15;j++) {
			directory_blockptr_inode_entry[indata[i].block_pt[j]].push_back(std::pair<unsigned long, unsigned long>(indata[i].inode_num, j));
		}
	}

	//unallocated block & duplicated block
	std::map<unsigned long, std::map<unsigned long, std::vector<unsigned long>>> inode_block_entry;//inode as key and get a map that use block num as key to achieve a vector of entry num
	for (size_t i = 0; i < indata.size(); i++) {
		unsigned long in_num = indata[i].inode_num;
		inode_block_entry[in_num] = get_block_list(indata[i], indirectdata);
	}

	//std::vector<unsigned long> block_bit_map_block_num;
	//output unallocated block entries
	for (size_t i = 0; i < groupdata.size(); i++) {
		//block_bit_map_block_num.push_back(groupdata[i].block_bit_block);
		unsigned long bitmap_block_num = groupdata[i].block_bit_block;//block number of bitmap
																	  //iterate through every free entry on that bitmap
		for (auto it = bitdata.bit[bitmap_block_num].begin(), it_end = bitdata.bit[bitmap_block_num].end(); it != it_end; ++it) {
			//for every free entry search every block list of every inode
			//for (auto ater = inode_block_entry.begin(), ater_end = inode_block_entry.end(); ater != ater_end; ++ater) {
				//found a collision
				//auto hit = ater->second.find(*it);
				//if (hit != ater->second.end()) {
					//std::cout << "UNALLOCATED BLOCK < " << *it << " > REFERENCED BY INODE < " << ater->first << " > ENTRY < " << hit->second[0] << " >\n";
					//Do I need to output all the block if there are duplicated??
				//}
			//}
			//search through indir_child_parent_entry(indirect.csv) and inode.csv to see all the allocated block and check if there is a match
			std::map<unsigned long, std::vector<std::pair<unsigned long, unsigned long>>> inode_indirbl_entry_num;//map an inode to a pair, for direct data in the directory.csv will have indirbl=0
			auto hit_in_dir = directory_blockptr_inode_entry.find(*it);
			if (hit_in_dir != directory_blockptr_inode_entry.end()) {
				for (size_t z = 0;z < hit_in_dir->second.size();z++) {
					inode_indirbl_entry_num[hit_in_dir->second[z].first].push_back(std::pair<unsigned long, unsigned long>(0, hit_in_dir->second[z].second));
				}
			}
			auto hit_in_indir = indir_child_parent_entry.find(*it);
			if (hit_in_indir != indir_child_parent_entry.end()) {
				//find a hit in indirect.csv and now we need to determine which inode it belongs to
				for (size_t z = 0;z < hit_in_indir->second.size();z++) {
					//iterate through each pair and get the parent number then check which inode this parent block belongs to from inode_block_entry
					unsigned long parent_bl = hit_in_indir->second[z].first;
					unsigned long entry_num = hit_in_indir->second[z].second;
					//Now search for inode by iterate through every block list of every inode
					for (auto ater = inode_block_entry.begin(), ater_end = inode_block_entry.end();ater != ater_end;++ater) {
						auto this_block = ater->second.find(hit_in_indir->first);
						if (ater->second.find(parent_bl) != ater->second.end() && this_block != ater->second.end()) {
							//parent block and the block itself both belong to the inode, we check entry to see if they match, if they match, we can assume this inode points to these blocks
							bool in_the_inode = false;
							for (size_t y = 0; y < this_block->second.size();y++) {
								if (this_block->second[y] == entry_num) {
									in_the_inode = true;
									break;
								}
							}
							if (in_the_inode == true) {
								//we find the owner inode of the block
								unsigned long this_bl_inode = ater->first;
								inode_indirbl_entry_num[this_bl_inode].push_back(std::pair<unsigned long, unsigned long>(parent_bl, entry_num));
							}
						}
					}
				}
			}
			//now we have all the info in the map and we can out put now
			if (inode_indirbl_entry_num.size() != 0) {
				std::string holder = "UNALLOCATED BLOCK < " + std::to_string(*it) + " > REFERENCED BY";
				for (auto ater = inode_indirbl_entry_num.begin(), ater_end = inode_indirbl_entry_num.end();ater != ater_end; ++ater) {
					unsigned long the_inode = ater->first;
					for (size_t x = 0;x < ater->second.size();x++) {
						if (ater->second[x].first == 0) {
							//direct data block
							holder.append(" INODE < " + std::to_string(the_inode) + " > ENTRY < " + std::to_string(ater->second[x].second) + " >");
						}
						else
							holder.append(" INODE < " + std::to_string(the_inode) + " > INDIRECT BLOCK < " + std::to_string(ater->second[x].first) + " > ENTRY < " + std::to_string(ater->second[x].second) + " >");
					}
				}
				ofile << holder << "\n";
			}
		}
	}

	//output duplicated block entries
	std::vector<unsigned long> dup_bl_num;
	for (auto it = inode_block_entry.begin(), it_end = inode_block_entry.end(); it != it_end; ++it) {
		const auto& block_list = it->second;
		for (auto ater = block_list.begin(), ater_end = block_list.end(); ater != ater_end; ++ater) {
			std::map<unsigned long, std::vector<unsigned long>> inode_entry;
			unsigned long block_num = ater->first;
			bool already_output = false;
			for (size_t i = 0; i < dup_bl_num.size(); i++) {
				if (block_num == dup_bl_num[i]) {
					already_output = true;
					break;
				}
			}
			if (already_output == true)continue;
			auto it_2 = it;
			std::string holder;
			for (++it_2; it_2 != it_end; ++it_2) {
				auto result = it_2->second.find(block_num);
				if (result != it_2->second.end()) {
					
					
					inode_entry.insert(std::pair<unsigned long, std::vector<unsigned long>>(it_2->first, result->second));
				}
			}
		
			if (inode_entry.size() != 0) {
				inode_entry.insert(std::pair<unsigned long, std::vector<unsigned long>>(it->first, ater->second));
				holder = "MULTIPLY REFERENCED BLOCK < " + std::to_string(block_num) + " > BY";
				for (auto deter = inode_entry.begin(), deter_end = inode_entry.end(); deter != deter_end; ++deter) {
				  


				  unsigned long inode_num = deter->first;
				  //first look into directory.csv to get rid of the entry_num in that and output it
				  auto in_directory = directory_blockptr_inode_entry.find(block_num);
				  if (in_directory != directory_blockptr_inode_entry.end()){
				    const auto& vec_of_dir = in_directory->second;
				    for (size_t z = 0; z < vec_of_dir.size(); z++){
				      if (vec_of_dir[z].first == inode_num){
					unsigned long entry_num = vec_of_dir[z].second;
					for (auto eter = deter->second.begin(), eter_end = deter->second.end(); eter != eter_end; ++eter){
					  if (entry_num == *eter){
					    //output this entry number
					    holder.append(" INODE < " + std::to_string(inode_num) + " > ENTRY < " + std::to_string(entry_num) + " >");
					    deter->second.erase(eter);//pop the entry_num out of list since it is outputed and then break out of loop since this loop might be garbage since we erase one of the element
					    break;
					  }
					}
				      }
				    }
				  }
				  if (deter->second.size()>0){
				    //look into indirect.csv, the rest of entry must come from there, we just need to find the parent block
				    unsigned long parent_block_num;
				    auto in_indir = indir_child_parent_entry.find(block_num);
				    if (in_indir != indir_child_parent_entry.end()){
				      const auto& vec_of_indir = in_indir->second;
				      for (size_t z = 0; z < vec_of_indir.size(); z++){
					unsigned long entry_num = vec_of_indir[z].second;
					parent_block_num = vec_of_indir[z].first;
					//make sure the entry_num is in the vector of entry_num we are going to output
					for (auto eter = deter->second.begin(), eter_end = deter->second.end(); eter != eter_end; ++eter){
					  if (entry_num == *eter){
					    //now we check one more step that the parent block and child block are indeed belong to this inode number from inode_block_entry, if true, we can assume it is safe to output
					    const auto& block_list_of_inode = inode_block_entry.at(inode_num);
					    auto parent_found = block_list_of_inode.find(parent_block_num);
					    auto child_found = block_list_of_inode.find(block_num);
					    if (parent_found != block_list_of_inode.end() && child_found != block_list_of_inode.end()){
					      //safe to output
					      holder.append(" INODE < " + std::to_string(inode_num) + " > INDIRECT BLOCK < " + std::to_string(parent_block_num) + " > ENTRY < " + std::to_string(entry_num) + " >");
					      deter->second.erase(eter);
					      break;
					    }
					  }
					}
				      }
				    }
				  }








				  
				}
			}
			if (!holder.empty()) {
				holder.append("\n");
				dup_bl_num.push_back(block_num);
				ofile << holder;
			}

		}
	}

	//child_inode, <parent(dir)_inode,entry number>
	std::map<unsigned long, std::map<unsigned long, std::vector<unsigned long>>> dir_entry;
	for (size_t i = 0; i < dirdata.size(); i++) {
		unsigned long child_inode = dirdata[i].inode_num;
		unsigned long dir_inode = dirdata[i].parent_inode_num;
		unsigned long entry_num = dirdata[i].entry_num;

		dir_entry[child_inode][dir_inode].push_back(entry_num);
	}

	for (auto it = dir_entry.begin(), it_end = dir_entry.end(); it != it_end; ++it) {
		auto inode_from_bitmap = inode_block_entry.find(it->first);
		if (inode_from_bitmap == inode_block_entry.end()) {
			std::string holder;
			holder = "UNALLOCATED INODE < " + std::to_string(it->first) + " > REFERENCED BY";
			for (auto ater = it->second.begin(), ater_end = it->second.end(); ater != ater_end; ++ater) {
				for (size_t z = 0; z < ater->second.size(); z++) {
					holder.append(" DIRECTORY < " + std::to_string(ater->first) + " > ENTRY < " + std::to_string(ater->second[z]) + " >");
				}
			}
			ofile << holder << "\n";

		}
	}


	for (auto it = inode_block_entry.begin(), it_end = inode_block_entry.end(); it != it_end; ++it) {
		auto result = dir_entry.find(it->first);
		if (result == dir_entry.end() && it->first >= 11) {
			unsigned long in_num = it->first;
			unsigned long inode_bitmap_group = (in_num - 1) / superdata[0].inodes_per_group;
			unsigned long inode_bitmap = groupdata[inode_bitmap_group].inode_bit_block;
			ofile << "MISSING INODE < " << in_num << " > SHOULD BE IN FREE LIST < " << inode_bitmap << " >\n";

		}
	}

	//mapping inodes to link count
	std::map<unsigned long, unsigned long> inode_lk_count;
	for (size_t i = 0; i < indata.size(); i++) {
		inode_lk_count[indata[i].inode_num] = indata[i].link_count;
	}

	for (auto it = dir_entry.begin(), it_end = dir_entry.end(); it != it_end; ++it) {
		unsigned long in_num = it->first;
		unsigned long sum_lk = 0;
		for (auto ater = it->second.begin(), ater_end = it->second.end(); ater != ater_end; ++ater) {
			sum_lk += ater->second.size();
		}
		auto res = inode_lk_count.find(in_num);
		unsigned long lk_co;
		if (res != inode_lk_count.end()) {
			lk_co = res->second;
		}
		else continue;
		if (lk_co != sum_lk) {
			ofile << "LINKCOUNT < " << in_num << " > IS < " << lk_co << " > SHOULD BE < " << sum_lk << " >\n";

		}
	}

	//output incorrect directory entry
	std::map<unsigned long, std::map<unsigned long, std::vector<std::string>>> dir_parentinode_childinode_name;
	for (size_t i = 0;i < dirdata.size();i++) {
		unsigned long parent_num = dirdata[i].parent_inode_num;
		unsigned long child_num = dirdata[i].inode_num;
		std::string child_name = dirdata[i].name;
		dir_parentinode_childinode_name[parent_num][child_num].push_back(child_name);
	}

	for (auto it = dir_parentinode_childinode_name.begin(), it_end = dir_parentinode_childinode_name.end();it != it_end; ++it) {
		unsigned long this_dir_inode = it->first;
		//now find . and .. directory under thie entry;
		unsigned long inode_for_dot;
		unsigned long inode_for_dot_dot;
		for (auto ater = it->second.begin(),ater_end = it->second.end();ater != ater_end;++ater) {
			const auto& name_vec = ater->second;
			for (size_t z = 0;z < name_vec.size();z++) {
				if (name_vec[z] == ".") {
					inode_for_dot = ater->first;
				}
				if (name_vec[z] == "..") {
					inode_for_dot_dot = ater->first;
				}
			}
		}
		//std::cout << "inode of dir:" << this_dir_inode << " " << ". : " << inode_for_dot << ".. : " << inode_for_dot_dot<<"\n";
		//find the true parent and compare with inode_for_dot_dot
		unsigned long true_parent=0;
		if (this_dir_inode != 2) {
			
			for (auto ater = dir_parentinode_childinode_name.begin(), ater_end = dir_parentinode_childinode_name.end();ater != ater_end;++ater) {
				//found the inode
				//true_parent = 0;
				if (ater->first == this_dir_inode)continue;
				auto holder = ater->second.find(this_dir_inode);
				if ( holder != ater->second.end()) {
					//std::cout << "Get a hit in inode: " << ater->first << "\n";
					//for (size_t de = 0; de < holder->second.size(); de++){
						//std::cout << holder->second[de]<<" ";
					//}
					
					//if (holder->second.size() != 0 && holder->second[0] != ".."){
					for (size_t w = 0; w < holder->second.size(); w++){
						if (holder->second[w] == ".." || holder->second[w] == ".")continue;
						true_parent = ater->first;
						//std::cout << true_parent << "\n";
					}
					
						//break;
					//}
					
				}
				if (true_parent != 0)break;
			}
		}
		//now compare . and ..
		//exception case for root
		if (this_dir_inode == 2) {
			if (inode_for_dot != 2) {
				ofile << "INCORRECT ENTRY IN < " << 2 << " > NAME < . > LINK TO < " << inode_for_dot << " > SHOULD BE < " << 2 << " >\n";
			}
			if (inode_for_dot_dot != 2) {
				ofile << "INCORRECT ENTRY IN < " << 2 << " > NAME < .. > LINK TO < " << inode_for_dot_dot << " > SHOULD BE < " << 2 << " >\n";
			}
		}
		else {
			if (this_dir_inode != inode_for_dot) {
				ofile << "INCORRECT ENTRY IN < " << this_dir_inode << " > NAME < . > LINK TO < " << inode_for_dot << " > SHOULD BE < " << this_dir_inode << " >\n";
			}
			if (true_parent != inode_for_dot_dot) {
				ofile << "INCORRECT ENTRY IN < " << this_dir_inode << " > NAME < .. > LINK TO < " << inode_for_dot_dot << " > SHOULD BE < " << true_parent << " >\n";
			}
		}
	}


	//check invalid block num, i.e. either 0 or bigger than maxmum block number
	unsigned long max_block_num = superdata[0].blocks_count;
	for (auto it = inode_block_entry.begin(), it_end = inode_block_entry.end(); it != it_end; ++it){
		unsigned long inode_num = it->first;
		const auto& block_list = it->second;
		for (auto ater = block_list.begin(), ater_end = block_list.end(); ater != ater_end; ++ater){
			if (ater->first >= max_block_num || ater->first == 0){
				unsigned long invalid_block = ater->first;
				
				//output each entry num by verifying whether this entry is in inode.csv or indirect.csv
				for (size_t z = 0; z < ater->second.size(); z++){
					bool outputed = false;
					unsigned long entry_num = ater->second[z];
					auto is_indir = indir_child_parent_entry.find(invalid_block);
					if (is_indir != indir_child_parent_entry.end()){
					        //get a hit in the indir
						const auto& vec = is_indir->second;
						//find if this entry is indeed in this vector
						for (size_t x = 0; x < vec.size(); x++){
							if (vec[x].second == entry_num){
								//indeed in this vec
								unsigned long parent_num = vec[x].first;
								ofile << "INVALID BLOCK < " << invalid_block << " > IN INODE < " << inode_num << " > INDIRECT BLOCK < " << parent_num << " > ENTRY < " << entry_num << " >\n";
								outputed = true;
								break;
							}
						}
					}
					if (outputed)continue;
					//then this must be in the directory
					ofile << "INVALID BLOCK < " << invalid_block << " > IN INODE < " << inode_num << " > ENTRY < " << entry_num << " >\n";

				}

			}
		}


	}
	ofile.close();
}