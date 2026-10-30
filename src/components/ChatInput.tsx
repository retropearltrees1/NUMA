import { useState, useRef, type KeyboardEvent } from "react";

interface ChatInputProps {
  suggestions: string[];
  onSend: (text: string) => void;
}

export default function ChatInput({ suggestions, onSend }: ChatInputProps) {
  const [text, setText] = useState("");
  const [showSuggestions, setShowSuggestions] = useState(false);
  const inputRef = useRef<HTMLTextAreaElement>(null);

  const handleSend = () => {
    if (!text.trim()) return;
    onSend(text);
    setText("");
    setShowSuggestions(false);
    inputRef.current?.focus();
  };

  const handleKeyDown = (e: KeyboardEvent<HTMLTextAreaElement>) => {
    if (e.key === "Enter" && !e.shiftKey) {
      e.preventDefault();
      handleSend();
    }
  };

  const handleSuggestionClick = (s: string) => {
    onSend(s);
    setShowSuggestions(false);
    inputRef.current?.focus();
  };

  return (
    <div className="chat-input-area">
      {showSuggestions && (
        <div className="suggestion-panel">
          <div className="suggestion-panel-title">Suggestions rapides</div>
          <div className="suggestion-grid">
            {suggestions.map((s) => (
              <button
                key={s}
                className="suggestion-chip"
                onClick={() => handleSuggestionClick(s)}
              >
                {s}
              </button>
            ))}
          </div>
        </div>
      )}
      <div className="chat-input-row">
        <button
          className="suggestions-toggle"
          onClick={() => setShowSuggestions((v) => !v)}
          title="Afficher les suggestions"
          aria-label="Suggestions"
        >
          {showSuggestions ? "×" : "+"}
        </button>
        <textarea
          ref={inputRef}
          className="chat-input"
          placeholder="Tape ta question ou ton calcul…"
          value={text}
          onChange={(e) => setText(e.target.value)}
          onKeyDown={handleKeyDown}
          rows={1}
        />
        <button
          className="send-btn"
          onClick={handleSend}
          disabled={!text.trim()}
          title="Envoyer"
        >
          Envoyer
        </button>
      </div>
    </div>
  );
}
