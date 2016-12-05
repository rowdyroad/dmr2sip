import React, { Component } from 'react'
import { connect } from 'react-redux'
import * as Actions from '../../actions'
import {browserHistory} from 'react-router'
import {fromJS} from 'immutable'

export default (props) => {


  let mapState = (state) => {
   let retState =  {
                  ...props,
                  data: state.main.getIn([props.scope,'success']) ? state.main.getIn([props.scope,'response']).toJS() : null,
                  context: state.main.has(props.scope) ? state.main.get(props.scope).toJS() : null
              }

    return props.map ? {[props.map] : retState} : retState;
  }

  let mapDispatch = (dispatch) => {

    let retState = {
      actions: {
        fetch: (id) => {
          dispatch(Actions.Fetch(props.scope, props.apiUrl + "/" + id, {
            onSuccess:(data) => {
              return props.listScope ? Actions.ListItemUpdate([props.listScope,"response","data"], data, props.pkAttribute) : false
            }
          }));
        },
        setState:(state) => {
          dispatch(Actions.ObjectMerge(props.scope, state))
        },
        close: () => {
          browserHistory.replace(props.listUrl || "/");
        },
        notFound: () => {
          browserHistory.replace(props.listUrl || "/");
        },
        error: (error) => {
          browserHistory.replace(props.notFoundUrl || "/");
        },
        clear: () => {
          dispatch(Actions.ObjectRemove(props.scope));
        }
      }
    }

    return props.map ? {[props.map] : retState} : retState;
  }

  let mapMerge = (a,b,c) => {
    return {...c, ...fromJS(a).mergeDeep(fromJS(b)).toJS()}
  }

  class ItemProxy extends Component
{
    componentWillMount = () => {
        if (this.props.autoFetch) {
          this.props.actions.fetch(this.props.params[this.props.pkAttribute])
        }
    }

    componentWillUpdate = (next) => {
      if (next.data && next.route.staticName) {
        next.route.name = next.data.name;
        next.actions.setState({title: next.data.name});
      }

      if (next.context && next.context.error) {
        this.props.actions.error(next.context.error);
      }

      if (!next.context && this.props.context) {
        this.props.actions.error(404);
      }
    }

    componentWillUnmount = () => {
      this.props.actions.clear();
    }

    render = () => {

        return (
                    <div id={"item-fetcher_" + this.props.scope}>
                        {React.Children.map(this.props.children, (child)=> {
                          return React.cloneElement(child, {...this.props, ...child.props});
                        })}
                    </div>
              )
    }
}


 return connect(mapState, mapDispatch, props.map ? mapMerge : null)(ItemProxy)
}