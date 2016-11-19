#include "pch.h"
#include "PlayScreen.h"

using namespace std;
using namespace Library;

namespace Pong {
	PlayScreen::PlayScreen(std::shared_ptr<Library::ScreenManager> screenManager, 
		PlayerOptions player1Option, PlayerOptions player2Option) :
		GameScreen(screenManager), mGameState(GameState::Play)
	{
		GameScreen::mTransitionOnTime = 1000ms;
		GameScreen::mTransitionOffTime = 300ms;
		mPlayer1Options = player1Option;
		mPlayer2Options = player2Option;
	}

	void PlayScreen::Initialize()
	{
		Game * mGame = GetGame();

		mBall = make_shared<Ball>(*mGame, this);
		mComponents.push_back(mBall);

		mPlayer1 = make_shared<Paddle>(*mGame, this, mPlayer1Options);
		mComponents.push_back(mPlayer1);

		mPlayer2 = make_shared<Paddle>(*mGame, this, mPlayer2Options);
		mComponents.push_back(mPlayer2);

		mScoreBoard = make_shared<ScoreBoard>(*mGame, this);
		mComponents.push_back(mScoreBoard);

		mKeyboard = reinterpret_cast<KeyboardComponent*>(mGame->Services().GetService(KeyboardComponent::TypeIdClass()));

		GameScreen::Initialize();
	}

	void PlayScreen::OnExit()
	{
		
		ScreenManager().lock()->AddScreen(make_unique<MenuScreen>(mScreenManager.lock()), true);
		GameScreen::OnExit();
	}

	void PlayScreen::Draw(const Library::GameTime & gameTime)
	{
		ScreenManager().lock()->FadeScreenToBlack(TransitionPosition());
		GameScreen::Draw(gameTime);
	}

	void PlayScreen::Update(const Library::GameTime & gameTime, bool otherScreenHasFocus, bool coveredByOtherScreen)
	{
		//Manipulate game states pause, play, win etc
		if (mKeyboard->WasKeyPressedThisFrame(Keys::Space))
		{
			if (mGameState == GameState::Play)
			{
				setGameState(GameState::Paused);
			}
			else if (mGameState == GameState::Paused)
			{
				setGameState(GameState::Play);
			}
			else
			{
				Reset();
			}
		}
		else if (mGameState != GameState::Play && mKeyboard->WasKeyPressedThisFrame(Keys::Back))
		{
			ExitScreen();
		}

		GameScreen::Update(gameTime, otherScreenHasFocus, coveredByOtherScreen);
	}

	void PlayScreen::HandleInput(Library::ScreenInputManager & inputManager)
	{
		GameScreen::HandleInput(inputManager);
	}

	//Resets all game components
	void PlayScreen::Reset()
	{
		mBall->ResetBall();
		mPlayer1->ResetPaddle();
		mPlayer2->ResetPaddle();
		mScoreBoard->ResetScores();
		setGameState(GameState::Play);
	}

	void PlayScreen::setGameState(GameState state)
	{
		mGameState = state;
	}

	GameState & PlayScreen::getGameState()
	{
		return mGameState;
	}

	Ball * PlayScreen::getBall()
	{
		return mBall.get();
	}

	Paddle * PlayScreen::getPlayer1()
	{
		return mPlayer1.get();
	}

	Paddle * PlayScreen::getPlayer2()
	{
		return mPlayer2.get();
	}

	ScoreBoard * PlayScreen::getScoreBoard()
	{
		return mScoreBoard.get();
	}
}