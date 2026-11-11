# Sample data notes

`library-records-sample.csv` is the CSV supplied with the assignment. The application uses the versioned text file `data/library_data.txt` for its live, editable records.

The text seed file preserves the six book records and two borrowed statuses from the supplied CSV. The CSV did not contain member names, emails, loan IDs, or issue dates, so the seed file uses clearly labelled illustrative demo members and an illustrative issue date (`2026-08-01`) for the two active sample loans. Replace the demo details with real test data before using this for an actual library.

The app saves every successful catalogue/member/loan change to `data/library_data.txt`. Back up that file before experimenting with important data.
