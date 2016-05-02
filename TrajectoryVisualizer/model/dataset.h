//
// dataset.h
//

#ifndef DATASET_H
#define DATASET_H

#include <string>
#include <vector>
#include <functional>

#include "../utils/csv.h"

//@ObjectInfo - contain neccessary information about object
//For example, name, growth, weight...
//Must contain public field		size_t class_id;
//And correctly assign			ObjectInfo a = b;
template<class ObjectInfo>
class Dataset{
public:
    Dataset() {}
	//@csv_file - path to load csv
	//@parse - function for parse row from csv into ObjectInfo
	//It takes vector of parsed parameters from csv, and reference for ObjectInfo.
	//It must return class name the object belongs to
	//Return empty string to skip row
	Dataset(std::string csv_file, std::function<std::string(const std::vector<std::string>&, ObjectInfo&)> parse);

	void			pushDataset(std::string csv_file, std::function<std::string(const std::vector<std::string>&, ObjectInfo&)> parse);

	size_t			getDatasetSize() const;

	std::string		getClassName(size_t class_id) const;
	size_t			getClassCardinality(size_t class_id) const;
	size_t			getClassesCount() const;

	ObjectInfo&			operator[](int index)		{ return data[index]; }
	const ObjectInfo&	operator[](int index) const	{ return data[index]; }


	typedef typename std::vector<ObjectInfo>::value_type		value_type;
	typedef typename std::vector<ObjectInfo>::iterator			iterator;
	typedef typename std::vector<ObjectInfo>::const_iterator	const_iterator;
	typedef typename std::random_access_iterator_tag			iterator_category;

	iterator begin() { return data.begin(); }	
	iterator end()	 { return data.end(); }

	const_iterator begin() const { return data.begin(); }
	const_iterator end()   const { return data.end(); }
private:
	std::vector<ObjectInfo>		data;

	//get class members by class_id
	std::vector<std::string>	class_name;
	std::vector<size_t>			class_count;
};



template<class ObjectInfo>
Dataset<ObjectInfo>::Dataset(std::string csv_file, std::function<std::string(const std::vector<std::string>&, ObjectInfo&)> parse)
{
	pushDataset(csv_file, parse);
}

template<class ObjectInfo>
void Dataset<ObjectInfo>::pushDataset(std::string csv_file, std::function<std::string(const std::vector<std::string>&, ObjectInfo&)> parse)
{
    auto parsed_csv = utils::csvtools::read_csv(csv_file);

	for (auto row : parsed_csv){
		ObjectInfo obj;
		std::string cls_name = parse(row, obj);

		if (cls_name == "")
		{
			continue;
		}

		size_t id = find(class_name.begin(), class_name.end(), cls_name) - class_name.begin();

		bool class_exists = id < class_name.size();
		if (!class_exists)
		{
			class_name.push_back(cls_name);
			class_count.push_back(0);
		}

		obj.class_id = id;
		class_count[id]++;
		data.push_back(obj);
	}
}

template<class ObjectInfo>
size_t Dataset<ObjectInfo>::getDatasetSize() const
{
	return data.size();
}

template<class ObjectInfo>
std::string Dataset<ObjectInfo>::getClassName(size_t class_id) const
{
	return class_name[class_id];
}

template<class ObjectInfo>
size_t Dataset<ObjectInfo>::getClassCardinality(size_t class_id) const
{
	return class_count[class_id];
}

template<class ObjectInfo>
size_t Dataset<ObjectInfo>::getClassesCount() const
{
	return class_name.size();
}


#endif 
