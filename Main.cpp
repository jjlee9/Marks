// Mark.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#include <set>
#include <bitset>
#include <iostream>
#include <vector>

constexpr int DIMENSION = 5;
constexpr int MARKS = 3;

using row_t = std::bitset<DIMENSION>;
struct RowCompare
{
    bool operator ()(const row_t& lhs, const row_t& rhs) const
    {
        return lhs.to_ulong() < rhs.to_ulong();
    }
};
// all possible layouts for one row
using possible_t = std::set<row_t, RowCompare>;

// for one possible layout for multiple rows
using one_layout = std::vector<row_t>;
struct LayoutCompare
{
    bool operator ()(const one_layout& lhs, const one_layout& rhs) const
    {
        auto lhs_ulong = 0;
        for (const auto& i : lhs)
        {
            lhs_ulong = i.to_ulong() + (lhs_ulong << DIMENSION);
        }

        auto rhs_ulong = 0;
        for (const auto& i : rhs)
        {
            rhs_ulong = i.to_ulong() + (rhs_ulong << DIMENSION);
        }

        return lhs_ulong < rhs_ulong;
    }
};
using output_t = std::set<one_layout, LayoutCompare>;

possible_t all_possible_in_one_row()
{
    // previous possible for (i-1)-th column
    possible_t pre_pos;
    // current possible for i-th column
    possible_t cur_pos;

    // init state
    pre_pos.emplace(std::bitset<DIMENSION>{});

    for (int i = 0; i < DIMENSION; ++i)
    {
        // for one row only, process the i-th column
        for (const auto& e : pre_pos)
        {
            auto marks = static_cast<int>(e.count());
            if (marks > MARKS)
            {
                // exceed the MARKS
                continue;
            }

            // add mark OFF elemnt for i-th column
            if (i != (DIMENSION - 1) || marks == MARKS)
            {
                cur_pos.insert(e);
            }
            if (marks == MARKS)
            {
                continue;
            }

            // add mark ON elemnt for i-th column
            auto o = e;
            o.set(i);
            marks = static_cast<int>(o.count());
            if (i == (DIMENSION - 1) && marks != MARKS)
            {
                // we are in the last iteration but marks is not our wanted target
                continue;
            }

            cur_pos.insert(std::move(o));
        }

        pre_pos = cur_pos;
        if (i != (DIMENSION - 1))
        {
            cur_pos.clear();
        }
    }

    return cur_pos;
}

void init(
    const possible_t& possible,
    output_t&         output)
{
    for (const auto& r : possible)
    {
        // for one possible row
        one_layout layout;
        layout.push_back(r);
        output.insert(std::move(layout));
    }
}

void generate(
    const int         row,
    const possible_t& possible,
    const output_t&   pre_output,
    output_t&         cur_output)
{
    if (row >= DIMENSION) { return; }

    output_t cur_output1;
    output_t& curr_output = (row == (DIMENSION - 1)) ? cur_output : cur_output1;

    for (const auto& r : possible)
    {
        // for one possible row
        for (const auto& l : pre_output)
        {
            // for one layout
            std::vector<int> column_marks(DIMENSION);

            for (const auto& r1 : l)
            {
                for (int i = 0; i < DIMENSION; ++i)
                {
                    if (r1[i] != 0)
                    {
                        ++column_marks[i];
                    }
                }
            }

            for (int i = 0; i < DIMENSION; ++i)
            {
                if (r[i] != 0)
                {
                    ++column_marks[i];
                }
            }

            bool ok = true;
            for (int i = 0; i < DIMENSION; ++i)
            {
                if (row == (DIMENSION - 1))
                {
                    if (column_marks[i] != MARKS)
                    {
                        ok = false;
                        break;
                    }
                }
                else
                {
                    if (column_marks[i] > MARKS)
                    {
                        ok = false;
                        break;
                    }
                }
            }
            if (ok)
            {
                auto layout = l;
                layout.push_back(r);
                curr_output.insert(std::move(layout));
            }
        }
    }

    generate(row + 1, possible, curr_output, cur_output);
}

int main()
{
    auto possible = all_possible_in_one_row();

    output_t pre_output, cur_output;
    init(possible, pre_output);
    generate(1, possible, pre_output, cur_output);

    return 0;
}
