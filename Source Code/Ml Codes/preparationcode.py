def prepare_data(dataframe):
    from azureml.training.tabular.preprocessing import data_cleaning
    
    logger.info("Running prepare_data")
    label_column_name = 'y'
    
    # extract the features, target and sample weight arrays
    y = dataframe[label_column_name].values
    X = dataframe.drop([label_column_name], axis=1)
    sample_weights = None
    X, y, sample_weights = data_cleaning._remove_nan_rows_in_X_y(X, y, sample_weights,
     is_timeseries=False, target_column=label_column_name)
    
    return X, y, sample_weights
