#pragma once
#include <iterator>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>

class CSVRow
{
public:
    std::string_view operator[](std::size_t index) const
    {
        return std::string_view(&m_line[m_data[index] + 1u], m_data[index + 1u] - (m_data[index] + 1u));
    }
    std::size_t size() const { return m_data.size() - 1; }
    void readNextRow(std::istream& str);
private:
    std::string m_line;
    std::vector<int> m_data;
};

std::istream& operator>>(std::istream& str, CSVRow& data);
