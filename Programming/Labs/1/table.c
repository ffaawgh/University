#include "table.h"

// separator types
enum SeparatorType
{
	Tab,
	LineBreak,
	End
};

// returns 1 if c is separator
int is_separator(char c)
{
	return c == '\t' ||
	       c == '\n' ||
	       c == '\0';
}

// returns 1 if c is linesbreaker
int is_linebreak(char c)
{
	return c == '\n' ||
	       c == '\0';
}

// goes to the beginning of the stream
void seek_begin(FILE* input_stream)
{
	fseek(input_stream, 0, SEEK_SET);
}

// returns number of columns in the table
// (in input_stream) 
int get_columns_count(FILE* input_stream)
{ 
  int columns_count = 1; 
 
  char c; 
  while (fscanf(input_stream, "%c", &c) != EOF &&
  	     !is_linebreak(c)) {
  	columns_count += is_separator(c); 
  } 
 
  return columns_count; 
}

// returns number of rows in the table
// (in input_stream) 
int get_rows_count(FILE* input_stream)
{
	int rows_count = 1;

	char c;
	while (fscanf(input_stream, "%c", &c) != EOF) {
		rows_count += is_linebreak(c);
	}

	return rows_count - 1;
}

// returns length of the current cell in chars.
// moves stream position on the beggining next
// cell if there is no linebreaker after this
// in another way moves stream carret on the 
// linebreaker
int get_cell_length(FILE* input_stream)
{
	int length = 0;

	char c;
	while (fscanf(input_stream, "%c", &c) != EOF &&
		   !is_separator(c)) {
		++length;
	}

	return length;
}

// reads cell as string in result
// returns type of the following by cell separator
enum SeparatorType read_cell(FILE* input_stream, int width, char* result)
{
	for (int i = 0; i < width; ++i) {
		fscanf(input_stream, "%c", result + i);
	}
	result[width] = '\0';

	if (feof(input_stream)) {
		return End;
	}
	char separator;
	fscanf(input_stream, "%c", &separator);
	switch (separator) {
		case '\t':
			return Tab;
		case '\n':
			return LineBreak;
		default:
			return End;
	}
}

// prints horizontal separator
void print_horizontal_sep(FILE* output_stream, Table* table)
{
	int cols_count = table->columns_count;

	fprintf(output_stream, "+");
	for (int i = 0; i < cols_count; ++i) {
		int width = get_width_column(table->columns[i]) + 2;
		for (int j = 0; j < width; ++j) {
			fprintf(output_stream, "-");
		}
		fprintf(output_stream, "+");
	}
	fprintf(output_stream, "\n");
}

// returns pointer to a Table object
// if read failed returns NULL
Table* create_table(FILE* input_stream)
{
	Table* result = (Table*)malloc(sizeof(Table));

	result->stream = input_stream;

	seek_begin(input_stream);
	int rows_count = result->rows_count = get_rows_count(input_stream);

	seek_begin(input_stream);
	int columns_count = result->columns_count = get_columns_count(input_stream);

	result->columns = (TableColumn**)malloc(sizeof(TableColumn*) * columns_count);
	for (int i = 0; i < columns_count; ++i) {
		result->columns[i] = create_table_column(rows_count);
	}

	TableColumn** columns = result->columns;

	int is_fail = 0;

	seek_begin(input_stream);
	for (int i = 0; i < rows_count; ++i) {
		for (int j = 0; j < columns_count; ++j) {
			if (!is_fail && feof(input_stream)) {
				fprintf(stderr, "Unexpected EOF (row %d, col %d)\n", i, j);
				is_fail = 1;
			}

			if (is_fail) {
				break;
			}

			size_t curr_pos = ftell(input_stream);
			int curr_width = get_cell_length(input_stream);

			char* cell_data = (char*)malloc(sizeof(char) * (curr_width + 1));
			fseek(input_stream, curr_pos, SEEK_SET);
			switch (read_cell(input_stream, curr_width, cell_data)) {
				case Tab:
					if (j >= columns_count - 1) {
						fprintf(stderr, "Expected end of line, but tab was received (row %d, col %d)\n", i, j);
						is_fail = 1;
					}
					break;
				case LineBreak:
					if (j != columns_count - 1) {
						fprintf(stderr, "Expected tab, but end of line was received (row %d, col %d)\n", i, j);
						is_fail = 1;
					}
					break;
				case End:
					if (j != columns_count - 1 && i != rows_count - 1) {
						fprintf(stderr, "Unexpected EOF (row %d, col %d)\n", i, j);
						is_fail = 1;
					}
					break;
			}

			if (!is_fail) {
				add_cell_column(columns[j], cell_data);
			}

			free(cell_data);
		}
	}

	if (is_fail) {
		free_table(result);
		return NULL;
	} else {
		return result;
	}
}

// prints the table to the stream in formatted view
void print_table(FILE* output_stream, Table* table)
{
	int rows_count = table->rows_count;
	int cols_count = table->columns_count;

	TableColumn** columns = table->columns;

	seek_begin(table->stream);

	print_horizontal_sep(output_stream, table);
	for (int i = 0; i < rows_count; ++i) {
		fprintf(output_stream, "| ");
		for (int j = 0 ; j < cols_count; ++j) {
			int curr_width = get_width_column(columns[j]);
			int curr_right_width = get_right_width_column(columns[j]);
			
			TableCell* cell = get_cell_column(columns[j], i);
			int curr_cell_width = get_width_cell(cell);
			char* curr_data = (char*)malloc(sizeof(char) * (curr_cell_width + 1));
			read_cell(table->stream, curr_cell_width, curr_data);
			int curr_data_length = get_width_cell(cell);

			int temp_int;
			double temp_double;

			switch (get_data_type_cell(cell)) {
				case Integer:
					sscanf(curr_data, "%d", &temp_int);
					fprintf(output_stream, "%*d%*s", curr_width - curr_right_width, temp_int, curr_right_width, "");
					break;
				case String:
					if (curr_data_length) {
						fprintf(output_stream, "%-*s", curr_width, curr_data);
					} else {
						fprintf(output_stream, "%-*s", curr_width, "None");
					}
					break;
				case Double:
					sscanf(curr_data, "%lf", &temp_double);
					fprintf(output_stream, "%*.*lf", curr_width, curr_right_width - 1, temp_double);
					break;
				case Character:
					fprintf(output_stream, "%*s%*s", (curr_width + 1) / 2, curr_data, curr_width / 2, "");
					break;
			}
			free(curr_data);

			fprintf(output_stream, " | ");
		}
		fprintf(output_stream, "\n");
		if (!i) {
			print_horizontal_sep(output_stream, table);
		}
	}
	print_horizontal_sep(output_stream, table);
}

// destroys Table by pointer to it
void free_table(Table* table)
{
	int columns_count = table->columns_count;
	TableColumn** column = table->columns;
	for (int i = 0; i < columns_count; ++i, ++column) {
		free_table_column(*column);
	}
	free(table->columns);
	free(table);
}
