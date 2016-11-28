import React, { Component } from 'react'
import { connect } from 'react-redux'
import * as Actions from '../../actions';

import {browserHistory} from 'react-router'
import Form from './Form';

class CreateForm extends Component
{
    componentWillMount = () =>
    {
        this.props.actions.clear();
    }
    componentWillUpdate = (next) => {
      if (next.created && !this.props.created) {
          browserHistory.replace("/points/" + next.created.point_id);
      }
    }
    render = () => {
        return <Form {...this.props}/>
    }
}

let mapData = (state) => {
    return {
            context: state.main.has('point') ? state.main.get('point').toJS() : null,
            data: state.form.hasIn(['point','values']) ? state.form.getIn(['point','values']).toJS() : null,
            created: state.main.hasIn(['point','response']) ? state.main.getIn(['point','response']).toJS() : null
    }
}

let mapDispatch = (dispatch) => {
  return {
    attributeKey: 'point_id',
    actions: {
      fetch: (id) => {
        dispatch(Actions.ObjectSet("point", {}));
      },
      save: (data) => {
        dispatch(Actions.Post("point", "/api/points", data, {
          onSuccess:Actions.ListItemAdd(["points","response","data"], data)
        }));
      },
      clear: () => {
        dispatch(Actions.ObjectRemove("point"));
      },
      close: (data) => {
        dispatch(Actions.ObjectRemove("point"));
        if (!data.point_id) {
           browserHistory.goBack();
         } else {
          browserHistory.replace("/points/" + data.point_id);
         }
      }

    }
  }
}

export default connect(mapData, mapDispatch)(CreateForm)
