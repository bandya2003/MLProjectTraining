config = AutoMLConfig( task="classification",
                       training_data=data,
                       label_column_name="label",
                       compute_target=compute_target,
                       enable_code_generation=True
                     )


# Disabling Code Generation
config = AutoMLConfig( task="classification", 
                       training_data=data,
                       label_column_name="label",
                       compute_target=compute_target,
                       enable_code_generation=False
                     )
