/* docsim_cosine.cc: cosine similarity
 *
 * Copyright 2007 Yung-chung Lin
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02110-1301 USA
 */

#include <map>
#include <math.h>

#include <xapian/docsim.h>
#include <xapian/document.h>

#include "omdebug.h"
#include "docsim_internal.h"

double
Xapian::DocSimCosine::calculate_similarity(const Xapian::Document &a,
                                           const Xapian::Document &b) const 
{
    Xapian::doccount doc_count = internal->db.get_doccount();

    std::map<std::string, double> wt_a;
    std::map<std::string, double> wt_b;

    double wt_a_denom = 0;
    double wt_b_denom = 0;

    Xapian::TermIterator titer;

    for (titer = a.termlist_begin(); titer != a.termlist_end(); ++titer) {
        double idf = log(doc_count / internal->db.get_termfreq(*titer));
        double tmp = (log(titer.get_wdf()) + 1.0) * idf;
        wt_a[*titer] = tmp;
        wt_a_denom += tmp * tmp;
    }

    for (titer = b.termlist_begin(); titer != b.termlist_end(); ++titer) {
        double idf = log(doc_count / internal->db.get_termfreq(*titer));
        double tmp = (log(titer.get_wdf()) + 1.0) * idf;
        wt_b[*titer] = tmp;
        wt_b_denom += tmp * tmp;
    }

    std::map<std::string, double>::iterator wt_iter;

    for (wt_iter = wt_a.begin(); wt_iter != wt_a.end(); ++wt_iter) {
        wt_iter->second /= wt_a_denom;
    }
    
    for (wt_iter = wt_b.begin(); wt_iter != wt_b.end(); ++wt_iter) {
        wt_iter->second /= wt_b_denom;
    }
    
    double wt_sq_sum_a = 0;
    double wt_sq_sum_b = 0;
    double inner_product = 0;

    for (wt_iter = wt_a.begin(); wt_iter != wt_a.end(); ++wt_iter) {
        wt_sq_sum_a += wt_iter->second * wt_iter->second;
        std::map<std::string, double>::iterator wt_iter2;
        wt_iter2 = wt_b.find(wt_iter->first);
        if (wt_iter2 != wt_b.end()) {
            inner_product += wt_iter->second * wt_iter->second;
        }
    }
    
    for (wt_iter = wt_b.begin(); wt_iter != wt_b.end(); ++wt_iter) {
        wt_sq_sum_b += wt_iter->second * wt_iter->second;
    }
    
    return inner_product / (sqrt(wt_sq_sum_a) * sqrt(wt_sq_sum_b));
}

std::string Xapian::DocSimCosine::get_description() const
{
    DEBUGCALL(INTRO, string, "Xapian::DocSimCosine::get_description", "");
    RETURN("Xapian::DocSimCosine()");
}
