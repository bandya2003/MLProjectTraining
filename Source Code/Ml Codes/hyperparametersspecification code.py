def generate_algorithm_config():
    from xgboost.sklearn import XGBClassifier
    
    algorithm = XGBClassifier(
        base_score=0.5,
        booster='gbtree',
        colsample_bylevel=1,
        colsample_bynode=1,
        colsample_bytree=1,
        gamma=0,
        learning_rate=0.1,
        max_delta_step=0,
        max_depth=3,
        min_child_weight=1,
        missing=numpy.nan,
        n_estimators=100,
        n_jobs=-1,
        nthread=None,
        objective='binary:logistic',
        random_state=0,
        reg_alpha=0,
        reg_lambda=1,
        scale_pos_weight=1,
        seed=None,
        silent=None,
        subsample=1,
        verbosity=0,
        tree_method='auto',
        verbose=-10
    )
    
    return algorithm
