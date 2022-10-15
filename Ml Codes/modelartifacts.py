best_run = remote_run.get_best_child()

best_run.download_file("outputs/generated_code/script.py", "script.py")
best_run.download_file("outputs/generated_code/script_run_notebook.ipynb", "script_run_notebook.ipynb")
