#include "CsvStreamer.h"


CsvStreamer::CsvStreamer(string filename, char delim)
{
	if (delim == '"')				// Quotes as delimiter? No bueno!
		delim = DEFAULT_DELIMITER;
	this->open(filename, delim);
}

CsvStreamer::CsvStreamer(string filename)
{
	CsvStreamer(filename, DEFAULT_DELIMITER);
}

CsvStreamer::CsvStreamer()
{
}

CsvStreamer::~CsvStreamer()
{
	this->close();
}

void CsvStreamer::close()
{
	this->file.close();
}


void CsvStreamer::open(string filename)
{
	this->open(filename, DEFAULT_DELIMITER);
}


void CsvStreamer::open(string filename, char delim)
{
	this->fields = 0;
	this->rows = 0;
	this->row_buffer.clear();
	this->delimiter = delim;
	this->file.close();
	this->file.open(filename.c_str(), ios::trunc);	// If file exists, resets it to empty
}


string CsvStreamer::sanitize(string str)
{
	size_t quote_pos = str.find('"');
	bool needs_quotes = quote_pos != string::npos
						|| str.find(this->delimiter) != string::npos
						|| str.find('\r') != string::npos
						|| str.find('\n') != string::npos
						|| str[0] == ' '
						|| str[str.length() - 1] == ' ';
	
	
	if (needs_quotes)
	{
		// First escape quotes by turning any " in the data into "" ...
		
		// (Note: The second condition is a failsafe against a bug that creeps up
		// sometimes in 64-bit; quote_pos may be at or near unsigned int max value,
		// yet apparently still != string::npos... causing of range errors.)
		while (quote_pos != string::npos && quote_pos < str.length())
		{
			str.insert(quote_pos, "\"");
			quote_pos = str.find('"', quote_pos + 2);	// Skip over these quotes and go to the next ones
		}
		
		// ... then wrap the data in quotes
		str = '"' + str + '"';
	}
	
	return str;
}

void CsvStreamer::add_field(string field_name)
{
	if (this->rows > 0)
		return;
	
	this->append(field_name);
	this->fields ++;
}

void CsvStreamer::save_fields()
{
	if (this->rows == 0)
		this->writeln();
}

void CsvStreamer::append(string data)
{
	this->append(data, true);
}

void CsvStreamer::append(string data, bool trim_spaces)
{
	data = trim_spaces ? trim(data) : data;
	data = this->sanitize(data);
	this->row_buffer.push_back(data);
}

void CsvStreamer::writeln()
{
	for (unsigned int i = 0; i < this->row_buffer.size(); i++)
	{
		if (i > 0)
			this->file << this->delimiter;
		this->file << this->row_buffer[i];
	}
	
	this->file << endl;
	
	this->row_buffer.clear();
	this->rows ++;
}


long CsvStreamer::row_count()
{
	return this->rows;
}


int CsvStreamer::field_count()
{
	return this->fields;
}
