import React, { Component } from 'react'
import { connect } from 'react-redux'
import * as Actions from '../../actions';
import * as UI from '../../components/UIKit';
import {browserHistory} from 'react-router'

import Form from './Form';

let mapData = (state, props) => {
    return {
            initialValues: props.data,
            data: state.form.hasIn(['point','values']) ? state.form.getIn(['point','values']).toJS() : null,
    }
}

let mapDispatch = (dispatch) => {
  return {

    actions: {
      fetch: (id) => {
        dispatch(Actions.Fetch("point", "/api/points/" + id));
      },
      save: (data) => {
        data = data.toJS();
        dispatch(Actions.Put("point", "/api/points/" + data.point_id, data, {
          onSuccess:Actions.ListItemUpdate(["points", "response", "data"], data, 'point_id')
        }));
      },
      close: () => {
        browserHistory.replace("/points");
      }
    }
  }
}

export default connect(mapData, mapDispatch)(Form)
